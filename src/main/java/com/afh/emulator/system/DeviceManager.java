package com.afh.emulator.system;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

public class DeviceManager {

  private final List<BusDevice> deviceList;

  public DeviceManager() {
    deviceList = new ArrayList<>();
  }

  public void addDevice(BusDevice newDevice) {
    for(BusDevice device : deviceList) {
      if(newDevice.isOverlapping(device)){
        throw new IllegalArgumentException("Device already overlapping with another device");
      }
    }
    this.deviceList.add(newDevice);
    int index = Collections.binarySearch(deviceList, newDevice, Comparator.comparingInt(BusDevice::getStart));

    if (index < 0) {
      index = -(index + 1);
    }

    deviceList.add(index, newDevice);
  }

  public void setData(int address, int input){
    BusDevice device = findBusDevice(address);
    device.setData(address, input);
  }

  public int getData(int address){
    BusDevice device = findBusDevice(address);
    return device.getData(address);
  }

  public void tick(boolean reset){
    this.deviceList.forEach(busDevice -> busDevice.tick(reset));
  }

  private BusDevice findBusDevice(int address) {
    int left = 0;
    int right = deviceList.size() - 1;

    while (left <= right) {
      int mid = left + (right - left) / 2;
      BusDevice device = deviceList.get(mid);

      if (address >= device.getStart() && address <= device.getEnd()) {
        return device;
      } else if (address < device.getStart()) {
        right = mid - 1;
      } else {
        left = mid + 1;
      }
    }

    return null;
  }
}
