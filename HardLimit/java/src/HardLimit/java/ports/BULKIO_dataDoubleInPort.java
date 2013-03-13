/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file distributed with this 
 * source distribution.
 * 
 * This file is part of REDHAWK Basic Components.
 * 
 * REDHAWK Basic Components is free software: you can redistribute it and/or modify it under the terms of 
 * the GNU Lesser General Public License as published by the Free Software Foundation, either 
 * version 3 of the License, or (at your option) any later version.
 * 
 * REDHAWK Basic Components is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
 * PURPOSE.  See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with this 
 * program.  If not, see http://www.gnu.org/licenses/.
 */
package HardLimit.java.ports;


import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Iterator;
import java.util.Map;
import org.omg.CORBA.TCKind;
import org.ossie.properties.AnyUtils;
import CF.DataType;
import java.util.ArrayDeque;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;
import HardLimit.java.HardLimit;
import BULKIO.*;

/**
 * @generated
 */
public class BULKIO_dataDoubleInPort extends dataDoublePOA {

    /**
     * @generated
     */
    protected HardLimit parent;
    
    /**
     * @generated
     */
    protected String name;
     
    /**
     * @generated
     */
    protected linkStatistics stats;

    /**
     * @generated
     */
    protected Object sriUpdateLock;

    /**
     * @generated
     */
    protected Object statUpdateLock;

    /**
     * @generated
     */
    protected Map<String, sriState> currentHs;

    /**
     * @generated
     */
    protected Object dataBufferLock;
    
    /**
     * @generated
     */
    protected int maxQueueDepth;
    
    /**
     * @generated
     */
    protected Semaphore queueSem;

    /**
     * @generated
     */
    protected Semaphore dataSem;

    /**
     * @generated
     */
    protected boolean blocking;
    
    /**
     * @generated
     */
    public BULKIO_dataDoubleInPort(HardLimit parent, String portName) 
    {
        this.parent = parent;
        this.name = portName;
        this.stats = new linkStatistics(this.name);
        this.sriUpdateLock = new Object();
        this.statUpdateLock = new Object();
        this.currentHs = new HashMap<String, sriState>();
        this.dataBufferLock = new Object();
        this.maxQueueDepth = 100;
        this.queueSem = new Semaphore(this.maxQueueDepth);
        this.dataSem = new Semaphore(0);
        this.blocking = false;
         
        //begin-user-code
        //end-user-code
    }

    /**
     * @generated
     */
    public void pushSRI(StreamSRI header) {
        synchronized (this.sriUpdateLock) {
            if (!this.currentHs.containsKey(header.streamID)) {
                this.currentHs.put(header.streamID, new sriState(header, true));
                if (header.blocking) {
                    //If switching to blocking we have to set the semaphore
                    synchronized (this.dataBufferLock) {
                        if (!blocking) {
                                try {
                                    queueSem.acquire(data.size());
                                } catch (InterruptedException e) {
                                    e.printStackTrace();
                                }    
                        }
                        blocking = true;
                    }
                }
            } else {
                StreamSRI oldSri = this.currentHs.get(header.streamID).getSRI();
                if (!parent.compareSRI(header, oldSri)) {
                    this.currentHs.put(header.streamID, new sriState(header, true));
                    if (header.blocking) {
                        //If switching to blocking we have to set the semaphore
                        synchronized (this.dataBufferLock) {
                            if (!blocking) {
                                    try {
                                        queueSem.acquire(data.size());
                                    } catch (InterruptedException e) {
                                        e.printStackTrace();
                                    }    
                            }
                            blocking = true;
                        }
                    }
                }
            }
        }

        //begin-user-code
        //end-user-code
    }

    /**
     * @generated
     */
    public void enableStats(boolean enable) {
        this.stats.setEnabled(enable);
    }

    /**
     * @generated
     */
    public PortStatistics statistics() {
        synchronized (statUpdateLock) {
            return this.stats.retrieve();
        }
    }

    /**
     * @generated
     */
    public PortUsageType state() {
        int queueSize = 0;
        synchronized (dataBufferLock) {
            queueSize = data.size();
	        if (queueSize == this.maxQueueDepth) {
	            return PortUsageType.BUSY;
	        } else if (queueSize == 0) {
	            return PortUsageType.IDLE;
	        }
	        return PortUsageType.ACTIVE;
	    }
    }

    /**
     * @generated
     */
    public StreamSRI[] activeSRIs() {
        synchronized (this.sriUpdateLock) {
            ArrayList<StreamSRI> sris = new ArrayList<StreamSRI>();
            Iterator<sriState> iter = this.currentHs.values().iterator();
            while(iter.hasNext()) {
                sris.add(iter.next().getSRI());
            }
            return sris.toArray(new StreamSRI[sris.size()]);
        }
    }
    
    /**
     * @generated
     */
    public int getCurrentQueueDepth() {
        synchronized (this.dataBufferLock) {
            return data.size();
        }
    }
    
    /**
     * @generated
     */
    public int getMaxQueueDepth() {
        synchronized (this.dataBufferLock) {
            return this.maxQueueDepth;
        }
    }
    
    /**
     * @generated
     */
    public void setMaxQueueDepth(int newDepth) {
        synchronized (this.dataBufferLock) {
            this.maxQueueDepth = newDepth;
            queueSem = new Semaphore(newDepth);
        }
    }

    /**
     * @generated
     */
    public String getName() {
        return this.name;
    }
     
    /**
     * A class to hold packet data.
     * @generated
     */
    public class Packet<T extends Object> {
        /** @generated */
        private final T data;
        /** @generated */
        private final PrecisionUTCTime time;
        /** @generated */
        private final boolean endOfStream;
        /** @generated */
        private final String streamID;
        /** @generated */
        private final StreamSRI H;
        /** @generated */
        private final boolean inputQueueFlushed;
        /** @generated */
        private final boolean sriChanged;
        
        /**
         * @generated
         */
        public Packet(T data, PrecisionUTCTime time, boolean endOfStream, String streamID, StreamSRI H, boolean sriChanged, boolean inputQueueFlushed) {
            this.data = data;
            this.time = time;
            this.endOfStream = endOfStream;
            this.streamID = streamID;
            this.H = H;
            this.inputQueueFlushed = inputQueueFlushed;
            this.sriChanged = sriChanged;
        };
        
        /**
         * @generated
         */
        public T getData() {
            return this.data;
        }
        
        /**
         * @generated
         */
        public PrecisionUTCTime getTime() {
            return this.time;
        }
        
        /**
         * @generated
         */
        public boolean getEndOfStream() {
            return this.endOfStream;
        }
        
        /**
         * @generated
         */
        public String getStreamID() {
            return this.streamID;
        }
        
        /**
         * @generated
         */
        public StreamSRI getSRI() {
            return this.H;
        }
        
        /**
         * This returns true if the input queue for the port was cleared because
         * the queue reached its size limit. The number of packets discarded
         * before this packet is equal to maxQueueDepth.
         * @generated
         */
        public boolean inputQueueFlushed() {
            return this.inputQueueFlushed;
        }
        
        /**
         * @generated
         */
        public boolean sriChanged() {
            return this.sriChanged;
        }
    };
    
    /**
     * This queue stores all packets received from pushPacket.
     * @generated
     */
    private ArrayDeque<Packet<double[]>> data = new  ArrayDeque<Packet<double[]>>();
    

    /**
     * @generated
     */
    public void pushPacket(double[] data, PrecisionUTCTime time, boolean endOfStream, String streamID) 
    {
        synchronized (this.dataBufferLock) {
            if (this.maxQueueDepth == 0) {
                return;
            }
        }
        
        boolean portBlocking = false;
        StreamSRI tmpH = new StreamSRI(1, 0.0, 1.0, (short)1, 0, 0.0, 0.0, (short)0, (short)0, streamID, false, new DataType[0]);
        boolean sriChanged = false;
        synchronized (this.sriUpdateLock) {
            if (this.currentHs.containsKey(streamID)) {
                tmpH = this.currentHs.get(streamID).getSRI();
                sriChanged = this.currentHs.get(streamID).isChanged();
                this.currentHs.get(streamID).setChanged(false);
                portBlocking = blocking;
            }
        }

        //begin-user-code
        //end-user-code

        // determine whether to block and wait for an empty space in the queue
        Packet<double[]> p = null;

        if (portBlocking) {
            p = new Packet<double[]>(data, time, endOfStream, streamID, tmpH, sriChanged, false);

            try {
                queueSem.acquire();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            synchronized (this.dataBufferLock) {
                this.stats.update(data.length, this.data.size()/this.maxQueueDepth, endOfStream, streamID, false);
                this.data.add(p);
                this.dataSem.release();
            }
        } else {
            synchronized (this.dataBufferLock) {
	            if (this.data.size() == this.maxQueueDepth) {
	                this.data.clear();
	                p = new Packet<double[]>(data, time, endOfStream, streamID, tmpH, sriChanged, true);
	                this.stats.update(data.length, 0, endOfStream, streamID, true);
	            } else {
                    p = new Packet<double[]>(data, time, endOfStream, streamID, tmpH, sriChanged, false);
	                this.stats.update(data.length, this.data.size()/this.maxQueueDepth, endOfStream, streamID, false);
	            }
	            this.data.add(p);
	            this.dataSem.release();
	        }
        }

        //begin-user-code
        //end-user-code 
        return;
    }
     
    /**
     * @generated
     */
    public Packet<double[]> getPacket(long wait) 
    {
        //begin-user-code
        //end-user-code
        try {
            if (wait < 0) {
                this.dataSem.acquire();
            } else {
                this.dataSem.tryAcquire(wait, TimeUnit.MILLISECONDS);
            }
        } catch (InterruptedException ex) {
            return null;
        }
        
        Packet<double[]> p = null;
        synchronized (this.dataBufferLock) {
            p = this.data.poll();
        }

        if (p != null) {
            if (p.getEndOfStream()) {
                synchronized (this.sriUpdateLock) {
                    if (this.currentHs.containsKey(p.getStreamID())) {
                        sriState rem = this.currentHs.remove(p.getStreamID());

                        if (rem.getSRI().blocking) {
                            boolean stillBlocking = false;
                            Iterator<sriState> iter = currentHs.values().iterator();
                            while (iter.hasNext()) {
                            	if (iter.next().getSRI().blocking) {
                                    stillBlocking = true;
                                    break;
                                }
                            }

                            if (!stillBlocking) {
                                blocking = false;
                            }
                        }
                    }
                }
            }
            
            if (blocking) {
                queueSem.release();
            }
        }

        return p;
    }

    /**
     * @generated
     */
	public class sriState {
		/** @generated */
        protected StreamSRI sri;
        /** @generated */
        protected boolean changed;
        
        /**
         * @generated
         */
        public sriState(StreamSRI sri, boolean changed) {
        	this.sri = sri;
        	this.changed = changed;
        }
        
        /**
         * @generated
         */
        public StreamSRI getSRI() {
        	return this.sri;
        }
        
        /**
         * @generated
         */
        public boolean isChanged() {
        	return this.changed;
        }
        
        /**
         * @generated
         */
        public void setSRI(StreamSRI sri) {
        	this.sri = sri;
        }
        
        /**
         * @generated
         */
        public void setChanged(boolean changed) {
        	this.changed = changed;
        }
	}

    /**
     * @generated
     */
    public class statPoint {
        /** @generated */
        int elements;
        /** @generated */
        float queueSize;
        /** @generated */
        double secs;
        /** @generated */
        double usecs;
    }

    /**
     * @generated
     */
    public class linkStatistics {
        /** @generated */
        protected boolean enabled;
        /** @generated */
        protected double bitSize;
        /** @generated */
        protected PortStatistics runningStats;
        /** @generated */
        protected statPoint[] receivedStatistics;
        /** @generated */
        protected List< String > activeStreamIDs;
        /** @generated */
        protected int historyWindow;
        /** @generated */
        protected int receivedStatistics_idx;
        /** @generated */
        protected double flushTime;
        /** @generated */
        protected String portName;
        
        /**
         * @generated
         */
        public linkStatistics(String portName) {
            this.enabled = true;
            this.bitSize = 8.0 * 8.0;
            this.historyWindow = 10;
            this.flushTime = 0.0;
            this.receivedStatistics_idx = 0;
            this.receivedStatistics = new BULKIO_dataDoubleInPort.statPoint[historyWindow];
            this.activeStreamIDs = new ArrayList<String>();
            this.portName = portName;
            this.runningStats = new PortStatistics();
            this.runningStats.portName = this.portName;
            this.runningStats.elementsPerSecond = -1.0f;
            this.runningStats.bitsPerSecond = -1.0f;
            this.runningStats.callsPerSecond = -1.0f;
            this.runningStats.averageQueueDepth = -1.0f;
            this.runningStats.streamIDs = new String[0];
            this.runningStats.timeSinceLastCall = -1.0f;
            this.runningStats.keywords = new DataType[0];
            for (int i = 0; i < historyWindow; ++i) {
                this.receivedStatistics[i] = new BULKIO_dataDoubleInPort.statPoint();
            }
        }

        /**
         * @generated
         */
        public void setBitSize(double bitSize) {
            this.bitSize = bitSize;
        }

        /**
         * @generated
         */
        public void setEnabled(boolean enableStats) {
            this.enabled = enableStats;
        }

        /**
         * @generated
         */
        public void update(int elementsReceived, float queueSize, boolean EOS, String streamID, boolean flush) {
            if (!this.enabled) {
                return;
            }
            double currTime = System.currentTimeMillis() / 1000.0;
            this.receivedStatistics[this.receivedStatistics_idx].elements = elementsReceived;
            this.receivedStatistics[this.receivedStatistics_idx].queueSize = queueSize;
            this.receivedStatistics[this.receivedStatistics_idx++].secs = currTime;
            if (!EOS) {
                if (!this.activeStreamIDs.contains(streamID)) {
                    this.activeStreamIDs.add(streamID);
                }
            } else {
                this.activeStreamIDs.remove(streamID);
            }
            this.receivedStatistics_idx = this.receivedStatistics_idx % this.historyWindow;
            if (flush) {
                this.flushTime = currTime;
            }
        }

        /**
         * @generated
         */
        public PortStatistics retrieve() {
            if (!this.enabled) {
                return null;
            }
            double secs = System.currentTimeMillis() / 1000.0;
            int idx = (this.receivedStatistics_idx == 0) ? (this.historyWindow - 1) : (this.receivedStatistics_idx - 1);
            double front_sec = this.receivedStatistics[idx].secs;
            double totalTime = secs - this.receivedStatistics[this.receivedStatistics_idx].secs;
            double totalData = 0;
            float queueSize = 0;
            int startIdx = (this.receivedStatistics_idx + 1) % this.historyWindow;
            for (int i = startIdx; i != this.receivedStatistics_idx; ) {
                totalData += this.receivedStatistics[i].elements;
                queueSize += this.receivedStatistics[i].queueSize;
                i = (i + 1) % this.historyWindow;
            }
            int receivedSize = receivedStatistics.length;
            synchronized (this.runningStats) {
                this.runningStats.timeSinceLastCall = (float)(secs - front_sec);
                this.runningStats.bitsPerSecond = (float)((totalData * this.bitSize) / totalTime);
                this.runningStats.elementsPerSecond = (float)(totalData / totalTime);
                this.runningStats.averageQueueDepth = (float)(queueSize / receivedSize);
                this.runningStats.callsPerSecond = (float)((receivedSize - 1) / totalTime);
                this.runningStats.streamIDs = this.activeStreamIDs.toArray(new String[0]);
                if (flushTime != 0.0) {
                    double flushTotalTime = secs - this.flushTime;
                    this.runningStats.keywords = new DataType[1];
                    this.runningStats.keywords[0] = new DataType();
                    this.runningStats.keywords[0].id = "timeSinceLastFlush";
                    this.runningStats.keywords[0].value = AnyUtils.toAny(new Double(flushTotalTime), TCKind.tk_double);
                }
            }
            return this.runningStats;
        }
    }
    

}