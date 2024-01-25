package com.standardar.common;

public class Quaternion {


    private float x = 0.0F;
    private float y = 0.0F;
    private float z = 0.0F;
    private float w = 1.0F;

    public Quaternion() {
        this.x = 0.0f;
        this.y = 0.0f;
        this.z = 0.0f;
        this.w = 1.0f;
    }

    public Quaternion(Quaternion right) {
        this.x = right.x;
        this.y = right.y;
        this.z = right.z;
        this.w = right.w;
    }

    public Quaternion(float x, float y, float z, float w) {
        this.x = x;
        this.y = y;
        this.z = z;
        this.w = w;
    }

    public Quaternion(float[] array) {
        this.x = array[0];
        this.y = array[1];
        this.z = array[2];
        this.w = array[3];
    }

    public void identify(){
        this.x = 0.0f;
        this.y = 0.0f;
        this.z = 0.0f;
        this.w = 1.0f;
    }

    public float x() {
        return this.x;
    }

    public float y() {
        return this.y;
    }

    public float z() {
        return this.z;
    }

    public float w() {
        return this.w;
    }

    public void xAxis(float[] dest, int offset) {
        float[] invec = {1.0F, 0.0F, 0.0F};
        rotateVector(this, invec, 0, dest, offset);
    }

    public void yAxis(float[] dest, int offset) {
        float[] invec = {0.0F, 1.0F, 0.0F};
        rotateVector(this, invec, 0, dest, offset);
    }

    public void zAxis(float[] dest, int offset) {
        float[] invec = {0.0F, 0.0F, 1.0F};
        rotateVector(this, invec, 0, dest, offset);
    }

    public float[] xAxis() {
        float[] outvec = new float[3];
        float[] invec = {1.0F, 0.0F, 0.0F};
        rotateVector(this, invec, 0, outvec, 0);
        return outvec;
    }

    public float[] yAxis() {
        float[] outvec = new float[3];
        float[] invec = {0.0F, 1.0F, 0.0F};
        rotateVector(this, invec, 0, outvec, 0);
        return outvec;
    }

    public float[] zAxis() {
        float[] outvec = new float[3];
        float[] invec = {0.0F, 0.0F, 1.0F};
        rotateVector(this, invec, 0, outvec, 0);
        return outvec;
    }

    public static void rotateVector(Quaternion q, float[] inVec, int inOffset, float[] outVec, int outOffset) {

        float x = inVec[(inOffset + 0)];
        float y = inVec[(inOffset + 1)];
        float z = inVec[(inOffset + 2)];

        float qx = q.x();
        float qy = q.y();
        float qz = q.z();
        float qw = q.w();

        float qvx =  qw * x + qy * z - qz * y;
        float qvy =  qw * y + qz * x - qx * z;
        float qvz =  qw * z + qx * y - qy * x;
        float qvw = -qx * x - qy * y - qz * z;

        outVec[(outOffset + 0)] = (qvx * qw + qvw * -qx + qvy * -qz - qvz * -qy);
        outVec[(outOffset + 1)] = (qvy * qw + qvw * -qy + qvz * -qx - qvx * -qz);
        outVec[(outOffset + 2)] = (qvz * qw + qvw * -qz + qvx * -qy - qvy * -qx);
    }


    public Quaternion inverse() {
        return new Quaternion(-this.x, -this.y, -this.z, this.w);
    }

    public void toMatrix(float[] dest, int offset, int stride) {

        float xx = this.x * this.x;
        float xy = this.x * this.y;
        float xz = this.x * this.z;
        float xw = this.x * this.w;

        float yy = this.y * this.y;
        float yz = this.y * this.z;
        float yw = this.y * this.w;

        float zz = this.z * this.z;
        float zw = this.z * this.w;

        dest[(offset + 0 + stride * 0)] = 1 - 2 * (yy + zz);
        dest[(offset + 0 + stride * 1)] = 2 * (xy - zw);
        dest[(offset + 0 + stride * 2)] = 2 * (xz + yw);

        dest[(offset + 1 + stride * 0)] = 2 * (xy + zw);
        dest[(offset + 1 + stride * 1)] = 1 - 2 * (xx + zz);
        dest[(offset + 1 + stride * 2)] = 2 * (yz - xw);

        dest[(offset + 2 + stride * 0)] = 2 * (xz - yw);
        dest[(offset + 2 + stride * 1)] = 2 * (yz + xw);
        dest[(offset + 2 + stride * 2)] = 1 - 2 * (xx + yy);
    }
}
