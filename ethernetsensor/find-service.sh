#!/bin/bash

# Service type to search for
SERVICE_TYPE="_bellsensors._udp"

# Search for the service and retrieve IP address and port
service_info=$(avahi-browse -trp "$SERVICE_TYPE" | tail -n 2)

# Check if service_info is not empty
if [ -z "$service_info" ]; then
    echo "Error: Service '$SERVICE_TYPE' not found."
    exit 1
fi


# Extract the line containing the resolved service information
resolved_line=$(echo "$service_info" | grep -E "^=.*;$SERVICE_TYPE;")
echo "$resolved_line"

# Extract hostname, IP address, and port from service_info
hostname=$(echo "$resolved_line" | awk -F\; '{print $7}')
address=$(echo "$resolved_line" | awk -F\; '{print $8}')
port=$(echo "$resolved_line" | awk -F\; '{print $9}')

# Print discovered information (optional)
echo "Discovered Service:"
echo "Hostname: $hostname"
echo "IP Address: $address"
echo "Port: $port"

# Craft a UDP packet (echoing "Hello, World!" as an example payload)
echo "Sending UDP packet to $address:$port"
echo "HELLO" | nc -u -w1 "$address" "$port"

echo "Packet sent."
