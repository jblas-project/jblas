// --- BEGIN LICENSE BLOCK ---
/*
 * Copyright (c) 2009, Mikio L. Braun
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *
 *     * Neither the name of the Technische Universität Berlin nor the
 *       names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior
 *       written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// --- END LICENSE BLOCK ---

package org.jblas.util;

/**
 *
 */
public class Logger {
    public static final int ERROR = 5;
    public static final int WARNING = 4;
    public static final int INFO = 3;
    public static final int CONFIG = 2;
    public static final int DEBUG = 1;

    public static final String levelNames[] = {
        "DEBUG", "CONFIG", "INFO", "WARNING", "ERROR"
    };

    private static Logger theLogger = new Logger();
    private int level;

    private Logger() {
        level = INFO;
    }

    public static Logger getLogger() {
        return theLogger;
    }

    public void log(int messageLevel, String msg) {
        if (level <= messageLevel) {
            System.err.println("-- org.jblas " + levelNames[messageLevel - 1] + " "+ msg);
        }
    }

    public void debug(String msg) {
        log(DEBUG, msg);
    }

    public void config(String msg) {
        log(CONFIG, msg);
    }

    public void info(String msg) {
        log(INFO, msg);
    }

    public void warning(String msg) {
        log(WARNING, msg);
    }

    public void error(String msg) {
        log(ERROR, msg);
    }

    public void setLevel(int level) {
        this.level = level;
    }
}
