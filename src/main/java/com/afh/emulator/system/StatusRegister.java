package com.afh.emulator.system;

public class StatusRegister {
  //NV1BDIZC
  private boolean N;
  private boolean V;
  private boolean B;
  private boolean D;
  private boolean I;
  private boolean Z;
  private boolean C;

  private boolean[] bits;

  public StatusRegister() {
    this.bits = new boolean[8];
  }

  public void tick(boolean reset){
    N = bits[7] && !reset;
    V = bits[6] && !reset;
    B = bits[4] && !reset;
    D = bits[3] && !reset;
    I = bits[2] && !reset;
    Z = bits[1] && !reset;
    C = bits[0] && !reset;
  }

  public void setN(boolean n) {
    this.bits[7] = n;
  }

  public void setV(boolean v) {
    this.bits[6] = v;
  }

  public void setB(boolean b) {
    this.bits[4] = b;
  }

  public void setD(boolean d) {
    this.bits[3] = d;
  }

  public void setI(boolean i) {
    this.bits[2] = i;
  }

  public void setZ(boolean z) {
    this.bits[1] = z;
  }

  public void setC(boolean c) {
    this.bits[0] = c;
  }

  public boolean isB() {
    return B;
  }

  public boolean isC() {
    return C;
  }

  public boolean isZ() {
    return Z;
  }

  public boolean isI() {
    return I;
  }

  public boolean isD() {
    return D;
  }

  public boolean isV() {
    return V;
  }

  public boolean isN() {
    return N;
  }

}
