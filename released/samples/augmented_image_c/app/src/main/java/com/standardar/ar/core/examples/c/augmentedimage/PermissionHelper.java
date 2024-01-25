/*
 * Copyright 2018 Google Inc. All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.standardar.ar.core.examples.c.augmentedimage;

import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.provider.Settings;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;

/** Helper to ask camera permission. */
public class PermissionHelper {
  private static final String CAMERA_PERMISSION = Manifest.permission.CAMERA;
  private static final String WRITE_PERMISSION = Manifest.permission.WRITE_EXTERNAL_STORAGE;
  private static final String READ_PERMISSION   = Manifest.permission.READ_EXTERNAL_STORAGE;
  private static final int CAMERA_PERMISSION_CODE = 0;
  private static final int WRITE_PERMISSION_CODE = 1;

  /** Check to see we have the necessary permissions for this app. */
  public static boolean hasPermission(Activity activity) {
    return ContextCompat.checkSelfPermission(activity, CAMERA_PERMISSION)== PackageManager.PERMISSION_GRANTED
            && ContextCompat.checkSelfPermission(activity, WRITE_PERMISSION)== PackageManager.PERMISSION_GRANTED
            && ContextCompat.checkSelfPermission(activity, READ_PERMISSION)== PackageManager.PERMISSION_GRANTED;
  }

  /** Check to see we have the necessary permissions for this app, and ask for them if we don't. */
  public static void requestPermission(Activity activity) {
    ActivityCompat.requestPermissions(
            activity, new String[]{WRITE_PERMISSION, CAMERA_PERMISSION, READ_PERMISSION}, CAMERA_PERMISSION_CODE);
  }

  /** Check to see if we need to show the rationale for this permission. */
  public static boolean shouldShowRequestPermissionRationale(Activity activity) {
    return ActivityCompat.shouldShowRequestPermissionRationale(activity, CAMERA_PERMISSION);
  }

  /** Launch Application Setting to grant permission. */
  public static void launchPermissionSettings(Activity activity) {
    Intent intent = new Intent();
    intent.setAction(Settings.ACTION_APPLICATION_DETAILS_SETTINGS);
    intent.setData(Uri.fromParts("package", activity.getPackageName(), null));
    activity.startActivity(intent);
  }
}
