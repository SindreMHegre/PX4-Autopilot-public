#!/usr/bin/env python3
"""
MAVLink position setpoint test script for PX4 offboard control.

Take off manually first, then run this script. It will switch to OFFBOARD
mode and fly a square pattern relative to the drone's current position,
verifying that mc_nn_control or mc_raptor receives MAVLink position setpoints.

Usage:
    python test_mavlink_setpoints.py [--connect udp:127.0.0.1:14550] [--side 2.0] [--dwell 5.0]

Prerequisites:
    pip install pymavlink

PX4 parameters to set before testing with mc_nn_control:
    MC_NN_OFFB=1       (replace OFFBOARD mode with Neural Control)
    MC_NN_MANL_CTRL=0  (use external setpoints, not manual control)
    MAV_FWDEXTSP=1     (forward external setpoints from MAVLink to uORB)
    Then reboot.

PX4 parameters for testing with mc_raptor:
    MC_RAPTOR_ENABLE=1
    MC_RAPTOR_OFFB=1
    MAV_FWDEXTSP=1
    Then reboot.

Verification inside SITL console:
    listener trajectory_setpoint   (should update at ~10 Hz while script runs)
    listener vehicle_status        (nav_state should show OFFBOARD)
"""

import argparse
import sys
import time

try:
    from pymavlink import mavutil
except ImportError:
    print("Error: pymavlink not installed. Run: pip install pymavlink")
    sys.exit(1)

# MAVLink custom mode for PX4 OFFBOARD
PX4_CUSTOM_MAIN_MODE_OFFBOARD = 6

SETPOINT_RATE = 10.0  # Hz


def send_position_setpoint(conn, x: float, y: float, z: float, yaw: float = 0.0):
    """Send SET_POSITION_TARGET_LOCAL_NED (NED frame, z is down)."""
    type_mask = (
        mavutil.mavlink.POSITION_TARGET_TYPEMASK_VX_IGNORE |
        mavutil.mavlink.POSITION_TARGET_TYPEMASK_VY_IGNORE |
        mavutil.mavlink.POSITION_TARGET_TYPEMASK_VZ_IGNORE |
        mavutil.mavlink.POSITION_TARGET_TYPEMASK_AX_IGNORE |
        mavutil.mavlink.POSITION_TARGET_TYPEMASK_AY_IGNORE |
        mavutil.mavlink.POSITION_TARGET_TYPEMASK_AZ_IGNORE |
        mavutil.mavlink.POSITION_TARGET_TYPEMASK_YAW_RATE_IGNORE
    )
    conn.mav.set_position_target_local_ned_send(
        int(time.time() * 1000) & 0xFFFFFFFF,
        conn.target_system,
        conn.target_component,
        mavutil.mavlink.MAV_FRAME_LOCAL_NED,
        type_mask,
        x, y, z,
        0, 0, 0,
        0, 0, 0,
        yaw,
        0,
    )


def stream_setpoint_for(conn, x: float, y: float, z: float, duration: float, label: str = ""):
    """Stream a fixed setpoint at SETPOINT_RATE Hz for `duration` seconds."""
    if label:
        print(f"  -> {label}  NED=({x:.2f}, {y:.2f}, {z:.2f})")
    end = time.time() + duration
    interval = 1.0 / SETPOINT_RATE
    while time.time() < end:
        t = time.time()
        send_position_setpoint(conn, x, y, z)
        elapsed = time.time() - t
        if interval - elapsed > 0:
            time.sleep(interval - elapsed)


def wait_for_armed(conn, timeout: float = 60.0):
    """Block until the vehicle reports as armed."""
    print("Waiting for vehicle to be armed (take off manually)...")
    deadline = time.time() + timeout
    while time.time() < deadline:
        msg = conn.recv_match(type='HEARTBEAT', blocking=True, timeout=1.0)
        if msg and (msg.base_mode & mavutil.mavlink.MAV_MODE_FLAG_SAFETY_ARMED):
            print("Vehicle is armed.")
            return True
    print("Timed out waiting for arm.")
    return False


def get_current_position(conn, timeout: float = 5.0):
    """Return (x, y, z) from LOCAL_POSITION_NED, or None on timeout."""
    deadline = time.time() + timeout
    while time.time() < deadline:
        msg = conn.recv_match(type='LOCAL_POSITION_NED', blocking=True, timeout=1.0)
        if msg:
            return msg.x, msg.y, msg.z
    return None


def set_offboard_mode(conn):
    print("Switching to OFFBOARD mode...")
    conn.mav.command_long_send(
        conn.target_system, conn.target_component,
        mavutil.mavlink.MAV_CMD_DO_SET_MODE,
        0,
        mavutil.mavlink.MAV_MODE_FLAG_CUSTOM_MODE_ENABLED,
        PX4_CUSTOM_MAIN_MODE_OFFBOARD,
        0, 0, 0, 0, 0,
    )


def main():
    parser = argparse.ArgumentParser(
        description="MAVLink setpoint test — take off manually, then run this to fly a square"
    )
    parser.add_argument("--connect", default="udp:127.0.0.1:14550",
                        help="MAVLink connection string (default: udp:127.0.0.1:14550)")
    parser.add_argument("--side", type=float, default=2.0,
                        help="Square side length in meters (default: 2.0)")
    parser.add_argument("--dwell", type=float, default=5.0,
                        help="Seconds to hold each corner (default: 5.0)")
    args = parser.parse_args()

    print(f"Connecting to {args.connect} ...")
    conn = mavutil.mavlink_connection(args.connect)

    print("Waiting for heartbeat...")
    conn.wait_heartbeat(timeout=30)
    print(f"Heartbeat from system {conn.target_system}, component {conn.target_component}")

    if not wait_for_armed(conn):
        sys.exit(1)

    pos = get_current_position(conn)
    if pos is None:
        print("Error: could not get current position from LOCAL_POSITION_NED.")
        sys.exit(1)

    ox, oy, oz = pos
    s = args.side
    print(f"Current position: NED=({ox:.2f}, {oy:.2f}, {oz:.2f})")
    print(f"Square will be flown relative to this position (side={s}m, altitude held).")

    # Square corners relative to current position, clockwise in NED
    corners = [
        (ox,     oy,     oz, "Hold current position"),
        (ox + s, oy,     oz, "North"),
        (ox + s, oy + s, oz, "North-East"),
        (ox,     oy + s, oz, "East"),
        (ox,     oy,     oz, "Return to start"),
    ]

    # Pre-send setpoints at current position for 2s — PX4 requires a live stream
    # before it allows switching to OFFBOARD mode.
    print("\nPre-sending setpoints at current position for 2s...")
    stream_setpoint_for(conn, ox, oy, oz, duration=2.0)

    set_offboard_mode(conn)

    total = len(corners) * args.dwell
    print(f"\nFlying square: side={s}m, dwell={args.dwell}s/corner, ~{total:.0f}s total.")
    print("Press Ctrl+C to abort.\n")

    try:
        for x, y, z, label in corners:
            stream_setpoint_for(conn, x, y, z, duration=args.dwell, label=label)
    except KeyboardInterrupt:
        print("\nAborted by user.")

    print("Done. Switch back to manual mode to land.")


if __name__ == "__main__":
    main()
