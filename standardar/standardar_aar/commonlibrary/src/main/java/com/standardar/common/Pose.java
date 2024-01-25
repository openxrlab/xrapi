package com.standardar.common;

public class Pose {


    private final Quaternion quat;
    private final float[] translate;

    public Pose() {
        this.quat = new Quaternion();
        this.translate = new float[]{0.0f, 0.0f, 0.0f};
    }

    public Pose(float[] position, float[] rotation) {
        this(position[0], position[1], position[2], rotation[0], rotation[1], rotation[2], rotation[3]);
    }

    private Pose(float[] position, Quaternion rotation) {
        this.translate = position;
        this.quat = rotation;
    }

    private Pose(float x, float y, float z, float qx, float qy, float qz, float qw) {
        this.quat = new Quaternion(qx, qy, qz, qw);
        this.translate = new float[]{x, y, z};
    }

    public float tx() {
        return this.translate[0];
    }

    public float ty() {
        return this.translate[1];
    }

    public float tz() {
        return this.translate[2];
    }

    public float qx() {
        return this.quat.x();
    }

    public float qy() {
        return this.quat.y();
    }

    public float qz() {
        return this.quat.z();
    }

    public float qw() {
        return this.quat.w();
    }

    Quaternion getQuaternion() {
        return this.quat;
    }

    public void getTranslation(float[] dest, int offset) {
        dest[offset+0] = this.translate[0];
        dest[offset+1] = this.translate[1];
        dest[offset+2] = this.translate[2];
    }

    public void getRotationQuaternion(float[] dest, int offset) {
        dest[(offset + 0)] = this.quat.x();
        dest[(offset + 1)] = this.quat.y();
        dest[(offset + 2)] = this.quat.z();
        dest[(offset + 3)] = this.quat.w();
    }

    public void getXAxis(float[] dest, int offset) {
        this.quat.xAxis(dest, offset);
    }

    public void getYAxis(float[] dest, int offset) {
        this.quat.yAxis(dest, offset);
    }

    public void getZAxis(float[] dest, int offset) {
        this.quat.zAxis(dest, offset);
    }

    public float[] getXAxis() {
        return this.quat.xAxis();
    }

    public float[] getYAxis() {
        return this.quat.yAxis();
    }

    public float[] getZAxis() {
        return this.quat.zAxis();
    }

    public Pose inverse() {
        float[] outPos = new float[3];
        Quaternion outQuad = this.quat.inverse();
        Quaternion.rotateVector(outQuad, this.translate, 0, outPos, 0);

        outPos[0] = (-outPos[0]);
        outPos[1] = (-outPos[1]);
        outPos[2] = (-outPos[2]);

        return new Pose(outPos, outQuad);
    }

    public void getTransformedAxis(int axis, float scale, float[] dest, int offset) {
        switch (axis)
        {
            case 0:
            {
                getXAxis(dest, offset);
                break;
            }
            case 1:
            {
                getYAxis(dest, offset);
                break;
            }
            case 2:
            {
                getZAxis(dest, offset);
                break;
            }
        }

        dest[offset+0] =  dest[offset+0]*scale;
        dest[offset+1] =  dest[offset+1]*scale;
        dest[offset+2] =  dest[offset+2]*scale;
    }

    public void toMatrix(float[] destMat, int offset) {

        this.quat.toMatrix(destMat, offset, 4);

        destMat[(offset + 0 + 12)] = this.translate[0];
        destMat[(offset + 1 + 12)] = this.translate[1];
        destMat[(offset + 2 + 12)] = this.translate[2];

        destMat[(offset + 3)] = 0.0F;
        destMat[(offset + 7)] = 0.0F;
        destMat[(offset + 11)] = 0.0F;
        destMat[(offset + 15)] = 1.0F;
    }
}
