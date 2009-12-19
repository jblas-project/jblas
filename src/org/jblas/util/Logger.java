/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

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
            System.err.println("-- org.jblas " + levelNames[level + 1] + " "+ msg);
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
