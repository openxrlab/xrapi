package com.standardar.examples.c.handgesture;

import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.PorterDuff;
import android.view.SurfaceView;

import com.standardar.common.Vector3f;

public class DrawHelper {

    public DrawHelper() {

        mPaint.setColor(0xFFF73D3D);
        mPaint.setStyle(Paint.Style.STROKE);




        mThumbPaint.setColor(Color.parseColor("#3CF239"));
        mThumbPaint.setStyle(Paint.Style.FILL);

        mForeFingerPaint.setColor(Color.parseColor("#D226CD"));
        mForeFingerPaint.setStyle(Paint.Style.FILL);

        mMiddleFingerPaint.setColor(Color.parseColor("#FAF64B"));
        mMiddleFingerPaint.setStyle(Paint.Style.FILL);

        mRingPaint.setColor(Color.parseColor("#D01321"));
        mRingPaint.setStyle(Paint.Style.FILL);

        mLitterPaint.setColor(Color.parseColor("#111ED2"));
        mLitterPaint.setStyle(Paint.Style.FILL);

        m3DPaint.setColor(Color.BLACK);
        m3DPaint.setStyle(Paint.Style.FILL);
    }

    private Paint mPaint = new Paint();

    //1.thumb->拇指 2.forefinger->食指 3.middle finger->无名指 4.ring finger->无名指 5.little finger->小指
    private Paint mThumbPaint=new Paint();          //大拇指
    private Paint mForeFingerPaint=new Paint();     //食指
    private Paint mMiddleFingerPaint=new Paint();   //中指
    private Paint mRingPaint=new Paint();           //无名指
    private Paint mLitterPaint=new Paint();         //小拇指

    private Paint m3DPaint=new Paint();   //3D点的坐标

    private Matrix matrix = new Matrix();

    public void drawHands(Vector3f[] points2d, SurfaceView mOverlap, float viewportWidth, float viewportHeight , float screenHeight, float previewWidth, float previewHeight,int cameraRoatation){
        if (points2d != null && points2d.length > 0) {
            long start=System.currentTimeMillis();
            Canvas canvas = mOverlap.getHolder().lockCanvas();
            if (canvas == null) {
                return ;
            }

            canvas.drawColor(0, PorterDuff.Mode.CLEAR);
            canvas.save();


            for (int i=0;i<points2d.length;i++){
                HandRotationUtil.rotatePoints(points2d[i],(int)previewWidth,(int)previewHeight,false,cameraRoatation);

            }


            float dy = (screenHeight-viewportHeight)/2;
            matrix.setScale(viewportWidth/previewHeight,viewportHeight/previewWidth);
            matrix.postTranslate(0,dy);
            canvas.setMatrix(matrix);



            for (int i = 0; i < points2d.length; i++) {
                //canvas.drawRect(handInfos[i].handRect, mPaint);       //绘制手势矩形框

                drawkeyPoints(canvas, points2d, 12);
                drawLines(canvas, points2d, 6);

            }

            canvas.restore();
            long end=System.currentTimeMillis();

            if(end-start<30){
                try {
                    Thread.sleep(30-(end-start));
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            mOverlap.getHolder().unlockCanvasAndPost(canvas);

        }else {
            clearCanvas(mOverlap);
        }
    }

    private void  drawkeyPoints(Canvas canvas, Vector3f[] points2D, int radius) {

        if (points2D.length > 0 && points2D != null) {
            if (points2D.length >= 20) {

                //setPaintSize(5); //设置画笔大小

                //0-3 拇指
                for (int i = 0; i <= 3; i++) {
                    //canvas.drawPoint(handInfo.keyPoints[i].x,handInfo.keyPoints[i].y,mThumbPaint);
                    canvas.drawCircle(points2D[i].x, points2D[i].y, radius, mThumbPaint);
                }

                //4-7 食指
                for (int j = 4; j <= 7; j++) {
                    //canvas.drawPoint(handInfo.keyPoints[j].x,handInfo.keyPoints[j].y,mForeFingerPaint);
                    canvas.drawCircle(points2D[j].x, points2D[j].y, radius, mForeFingerPaint);
                }

                //8-11 中指
                for (int k = 8; k <= 11; k++) {
                    //canvas.drawPoint(handInfo.keyPoints[k].x,handInfo.keyPoints[k].y,mMiddleFingerPaint);
                    canvas.drawCircle(points2D[k].x, points2D[k].y, radius, mMiddleFingerPaint);
                }

                //12-15 无名指
                for (int q = 12; q <= 15; q++) {
                    //canvas.drawPoint(handInfo.keyPoints[q].x,handInfo.keyPoints[q].y,mRingPaint);
                    canvas.drawCircle(points2D[q].x, points2D[q].y, radius, mRingPaint);
                }

                //16-20
                for (int w = 16; w <= 19; w++) {
                    //canvas.drawPoint(handInfo.keyPoints[w].x,handInfo.keyPoints[w].y,mLitterPaint);
                    canvas.drawCircle(points2D[w].x, points2D[w].y, radius, mLitterPaint);
                }
            }
        }
    }

    private void drawLines(Canvas canvas,Vector3f[] points2D , int size){
        if(points2D!=null) {

            if (points2D.length >= 20) {
                setPaintSize(size);
                //0-3 拇指
                canvas.drawLine(points2D[0].x, points2D[0].y, points2D[1].x, points2D[1].y, mThumbPaint);
                canvas.drawLine(points2D[1].x, points2D[1].y, points2D[2].x, points2D[2].y, mThumbPaint);
                canvas.drawLine(points2D[2].x, points2D[2].y, points2D[3].x, points2D[3].y, mThumbPaint);

                //4-7 食指
                canvas.drawLine(points2D[0].x, points2D[0].y, points2D[4].x, points2D[4].y, mForeFingerPaint);
                canvas.drawLine(points2D[4].x, points2D[4].y, points2D[5].x, points2D[5].y, mForeFingerPaint);
                canvas.drawLine(points2D[5].x, points2D[5].y, points2D[6].x, points2D[6].y, mForeFingerPaint);
                canvas.drawLine(points2D[6].x, points2D[6].y, points2D[7].x, points2D[7].y, mForeFingerPaint);

                //8-11 中指
                canvas.drawLine(points2D[0].x, points2D[0].y, points2D[8].x, points2D[8].y, mMiddleFingerPaint);
                canvas.drawLine(points2D[8].x, points2D[8].y, points2D[9].x, points2D[9].y, mMiddleFingerPaint);
                canvas.drawLine(points2D[9].x, points2D[9].y, points2D[10].x, points2D[10].y, mMiddleFingerPaint);
                canvas.drawLine(points2D[10].x, points2D[10].y, points2D[11].x, points2D[11].y, mMiddleFingerPaint);

                //12-15 无名指
                canvas.drawLine(points2D[0].x, points2D[0].y, points2D[12].x, points2D[12].y, mRingPaint);
                canvas.drawLine(points2D[12].x, points2D[12].y, points2D[13].x, points2D[13].y, mRingPaint);
                canvas.drawLine(points2D[13].x, points2D[13].y, points2D[14].x, points2D[14].y, mRingPaint);
                canvas.drawLine(points2D[14].x, points2D[14].y, points2D[15].x, points2D[15].y, mRingPaint);

                //16-20 小指
                canvas.drawLine(points2D[0].x, points2D[0].y, points2D[16].x, points2D[16].y, mLitterPaint);
                canvas.drawLine(points2D[16].x, points2D[16].y, points2D[17].x, points2D[17].y, mLitterPaint);
                canvas.drawLine(points2D[17].x, points2D[17].y, points2D[18].x, points2D[18].y, mLitterPaint);
                canvas.drawLine(points2D[18].x, points2D[18].y, points2D[19].x, points2D[19].y, mLitterPaint);
            }
        }
    }

    private void  setPaintSize(int size){
        mThumbPaint.setStrokeWidth(size);
        mForeFingerPaint.setStrokeWidth(size);
        mMiddleFingerPaint.setStrokeWidth(size);
        mRingPaint.setStrokeWidth(size);
        mLitterPaint.setStrokeWidth(size);
        m3DPaint.setStrokeWidth(size);

    }


    /**
     * 清空画布
     * @param mOverlap 绘制手势框的SurfaceView
     */
    private void clearCanvas(SurfaceView mOverlap) {
        Canvas canvas = mOverlap.getHolder().lockCanvas();
        if (canvas != null) {
            canvas.drawColor(0, PorterDuff.Mode.CLEAR);
            mOverlap.getHolder().unlockCanvasAndPost(canvas);
        }
    }

}
