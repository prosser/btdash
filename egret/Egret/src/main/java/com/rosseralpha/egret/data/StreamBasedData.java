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

import java.io.ByteArrayInputStream;

public abstract class StreamBasedData {
    public abstract void Load(ByteArrayInputStream stream);

    protected static short ReadInt16(ByteArrayInputStream stream) {
        return (short)(stream.read() << 8 + stream.read());
    }

    protected static int ReadInt32(ByteArrayInputStream stream) {
        return stream.read() << 24 +
               stream.read() << 16 +
               stream.read() << 8 +
               stream.read();
    }

    protected static byte[] ReadBuffer(ByteArrayInputStream stream, int count) {
        byte[] buffer = new byte[count];
        stream.read(buffer, 0, count);
        return buffer;
    }

    protected static boolean ReadBoolean(ByteArrayInputStream stream) {
        return stream.read() == 0 ? true : false;
    }

    protected static float ReadFloat(ByteArrayInputStream stream) {
        return Float.intBitsToFloat(ReadInt32(stream));
    }
}
