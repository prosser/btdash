/*
 Copyright (c) 2013, ROSSER ALPHA LLC
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of ROSSER ALPHA LLC nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL ROSSER ALPHA LLC BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

package com.rosseralpha.egret.data;

/**
 * Created by peter on 6/23/13.
 */

import android.content.Context;
import android.hardware.usb.UsbManager;

import com.hoho.android.usbserial.driver.UsbSerialDriver;
import com.hoho.android.usbserial.driver.UsbSerialProber;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;

public class UsbSerialLoader {
    private UsbManager manager;
    private UsbSerialDriver driver;
    private Context context;
    private boolean connected = false;

    public UsbSerialLoader(Context context) throws IOException {
        this.context = context;
    }

    // connect to the arduino
    public void connect() throws IOException {
        // Get UsbManager from Android.
        manager = (UsbManager) context.getSystemService(Context.USB_SERVICE);

        // Find the first available driver.
        driver = UsbSerialProber.acquire(manager);

        if (driver != null) {
            driver.open();
            try {
                driver.setBaudRate(115200);
                driver.setParameters(115200, 8, UsbSerialDriver.STOPBITS_1, UsbSerialDriver.PARITY_NONE);

                driver.wait();

                int read;
                byte[] buffer = new byte[1000];

                do {
                    read = driver.read(buffer, 1000);
                    if (read > 0) {
                        String str = new String(buffer, "ASCII");
                        if (str.toLowerCase().startsWith("btdash")) {
                            String send = "C\n";

                            byte[] toSend = new byte[2];
                            toSend[0] = 'C';
                            toSend[1] = '\n';

                            driver.write(toSend, 2);

                            // cool! we have a talking Arduino
                            connected = true;
                            break;
                        }
                    }
                    else {
                        Thread.sleep(100);
                    }

                } while (true);
            }
            catch(Exception ex) {
                driver.close();
            }
        }
    }

    public byte[] read(int count) throws IOException {
        byte[] buffer = new byte[count];
        driver.read(buffer, count);
        return buffer;
    }

    public void dispose() {
        if (driver != null) {
            try {
                driver.close();
            }
            catch (IOException ex) {
                // do nothing
            }
            driver = null;
        }
    }
}
