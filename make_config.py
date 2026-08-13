#!/usr/bin/env python3
"""
make_config.py — builds the binary config block flashed to the ESP32's
"usrcfg" partition (offset 0x3E0000, see partitions.csv).

Must exactly match the DeviceConfig struct in the .ino:
    magic[4] + ssid[32] + password[64] + ip[4] + gateway[4] + subnet[4]
padded to the full 4096-byte partition size.
"""

import sys

PARTITION_SIZE = 4096


def build_config(ssid: str, password: str, ip: str, gateway: str, subnet: str) -> bytes:
    magic = b"CFG1"

    ssid_b = ssid.encode("utf-8")[:31].ljust(32, b"\x00")
    pass_b = password.encode("utf-8")[:63].ljust(64, b"\x00")

    def ip_bytes(s):
        parts = [int(x) for x in s.split(".")]
        if len(parts) != 4 or any(p < 0 or p > 255 for p in parts):
            raise ValueError(f"'{s}' is not a valid IPv4 address")
        return bytes(parts)

    ip_b = ip_bytes(ip)
    gw_b = ip_bytes(gateway)
    sn_b = ip_bytes(subnet)

    blob = magic + ssid_b + pass_b + ip_b + gw_b + sn_b
    if len(blob) > PARTITION_SIZE:
        raise ValueError("config block exceeds partition size")

    # 0xFF matches the erased-flash state, harmless padding
    return blob.ljust(PARTITION_SIZE, b"\xff")


def main():
    if len(sys.argv) != 7:
        print("Usage: make_config.py <ssid> <password> <ip> <gateway> <subnet> <output.bin>")
        sys.exit(1)

    ssid, password, ip, gateway, subnet, out_path = sys.argv[1:7]

    try:
        blob = build_config(ssid, password, ip, gateway, subnet)
    except ValueError as e:
        print(f"Error: {e}")
        sys.exit(1)

    with open(out_path, "wb") as f:
        f.write(blob)

    print(f"Config written to {out_path} ({len(blob)} bytes)")
    print(f"  SSID:    {ssid}")
    print(f"  IP:      {ip}")
    print(f"  Gateway: {gateway}")
    print(f"  Subnet:  {subnet}")


if __name__ == "__main__":
    main()
