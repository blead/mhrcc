# MHRCC

Multi-hop radio-controlled cars. Built using ESP-8266 (NodeMCU), with custom wireless ad-hoc communication protocol.
[demo video](https://www.youtube.com/watch?v=qV8OEQQNxMI).

## Controller Setup

1. Create a wireless ad-hoc network **with no security**. See [https://help.ubuntu.com/community/WifiDocs/Adhoc](https://help.ubuntu.com/community/WifiDocs/Adhoc) for an example.

2. Install dependencies.

  ```sh
  pip install pygame scapy
  ```

3. Set interface name in variable `INTF`.

4. Start the controller.

## Car Setup

Car is based on NodeMCU.

1. Set `wifi_channel`.

2. Set `self_id` (`0x61` for A, `0x62` for B).

3. Set `next_hop_id`.

4. Set `ap` to network BSSID. (This can be retrieved from running `iwconfig` on the controller.)
