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
package HardLimit.java;


import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Properties;
import org.omg.CORBA.ORB;
import org.omg.PortableServer.POA;
import org.omg.PortableServer.POAPackage.ServantNotActive;
import org.omg.PortableServer.POAPackage.WrongPolicy;
import org.omg.CosNaming.NamingContextPackage.CannotProceed;
import org.omg.CosNaming.NamingContextPackage.InvalidName;
import org.omg.CosNaming.NamingContextPackage.NotFound;
import CF.PropertiesHolder;
import CF.ResourceHelper;
import CF.UnknownProperties;
import CF.LifeCyclePackage.InitializeError;
import CF.LifeCyclePackage.ReleaseError;
import CF.InvalidObjectReference;
import CF.PropertySetPackage.InvalidConfiguration;
import CF.PropertySetPackage.PartialConfiguration;
import CF.ResourcePackage.StartError;
import CF.ResourcePackage.StopError;
import CF.DataType;
import org.omg.CORBA.UserException;
import org.omg.CosNaming.NameComponent;
import org.apache.log4j.Logger;
import org.ossie.component.*;
import org.ossie.properties.*;
import BULKIO.StreamSRI;
import HardLimit.java.ports.*;

/**
 * This is the component code. This file contains all the access points
 * you need to use to be able to access all input and output ports,
 * respond to incoming data, and perform general component housekeeping
 *
 * Source: HardLimit.spd.xml
 * Generated on: Thu Feb 21 13:40:17 EST 2013
 * Redhawk IDE
 * Version:M.1.8.2
 * Build id: v201211201139RC3 
 
 * @generated
 */
public class HardLimit extends Resource implements Runnable {
    /**
     * @generated
     */
    public final static Logger logger = Logger.getLogger(HardLimit.class.getName());
    
	/**
	 * The property upper_limit
	 * If the meaning of this property isn't clear, a description should be added.
	 * 
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public final SimpleProperty<Double> upper_limit =
		new SimpleProperty<Double>(
			"upper_limit", //id
			null, //name
			"double", //type
			1.0, //default value
			"readwrite", //mode
			"external", //action
			new String[] {"configure"} //kind
			);
    
	/**
	 * The property lower_limit
	 * If the meaning of this property isn't clear, a description should be added.
	 * 
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public final SimpleProperty<Double> lower_limit =
		new SimpleProperty<Double>(
			"lower_limit", //id
			null, //name
			"double", //type
			-1.0, //default value
			"readwrite", //mode
			"external", //action
			new String[] {"configure"} //kind
			); 
    // Provides/inputs
    /**
     * @generated
     */
    public BULKIO_dataDoubleInPort port_dataDouble_in;

    public class MyBULKIO_dataDoubleOutPort extends BULKIO_dataDoubleOutPort
    {
    	public MyBULKIO_dataDoubleOutPort(String portName) {
			super(portName);
		}

		public boolean hasSri(String streamID)
    	{
    		return this.currentSRIs.containsKey(streamID);
    	}
    };
    
    // Uses/outputs
    /**
     * @generated
     */
    public MyBULKIO_dataDoubleOutPort port_dataDouble_out;

    /**
     * @generated
     */
    public HardLimit() 
    {
        super();      
        addProperty(upper_limit);
        addProperty(lower_limit);

        // Provides/input
        this.port_dataDouble_in = new BULKIO_dataDoubleInPort(this, "dataDouble_in");
        this.addPort("dataDouble_in", this.port_dataDouble_in);

        // Uses/output
        this.port_dataDouble_out = new MyBULKIO_dataDoubleOutPort("dataDouble_out");
        this.addPort("dataDouble_out", this.port_dataDouble_out);
    
       //begin-user-code
       //end-user-code
    }

    
    /**
     * @generated
     */
    public boolean compareSRI(StreamSRI SRI_1, StreamSRI SRI_2){
        if (SRI_1.hversion != SRI_2.hversion)
            return false;
        if (SRI_1.xstart != SRI_2.xstart)
            return false;
        if (SRI_1.xdelta != SRI_2.xdelta)
            return false;
        if (SRI_1.xunits != SRI_2.xunits)
            return false;
        if (SRI_1.subsize != SRI_2.subsize)
            return false;
        if (SRI_1.ystart != SRI_2.ystart)
            return false;
        if (SRI_1.ydelta != SRI_2.ydelta)
            return false;
        if (SRI_1.yunits != SRI_2.yunits)
            return false;
        if (SRI_1.mode != SRI_2.mode)
            return false;
        if (SRI_1.streamID != SRI_2.streamID)
            return false;
        if (SRI_1.keywords.length != SRI_2.keywords.length)
            return false;
        String action = "eq";
        for (int i=0; i < SRI_1.keywords.length; i++) {
            if (!SRI_1.keywords[i].id.equals(SRI_2.keywords[i].id)) {
                return false;
            }
            if (!SRI_1.keywords[i].value.type().equivalent(SRI_2.keywords[i].value.type())) {
                return false;
            }
            if (AnyUtils.compareAnys(SRI_1.keywords[i].value, SRI_2.keywords[i].value, action)) {
                return false;
            }
        }
        return true;
    }

    /**
     *
     * Main processing thread
     *
     * <!-- begin-user-doc -->
     * 
     * General functionality:
     * 
     *    This function is running as a separate thread from the component's main thread. 
     *    
     *    The IDE uses JMerge during the generation (and re-generation) process.  To keep
     *    customizations to this file from being over-written during subsequent generations,
     *    put your customization in between the following tags:
     *      - //begin-user-code
     *      - //end-user-code
     *    or, alternatively, set the @generated flag located before the code you wish to 
     *    modify, in the following way:
     *      - "@generated NOT"
     * 
     * StreamSRI:
     *    To create a StreamSRI object, use the following code:
     *        this.stream_id = "stream";
     * 		  StreamSRI sri = new StreamSRI();
     * 		  sri.mode = 0;
     * 		  sri.xdelta = 0.0;
     * 		  sri.ydelta = 1.0;
     * 		  sri.subsize = 0;
     * 		  sri.xunits = 1; // TIME_S
     * 		  sri.streamID = (this.stream_id.getValue() != null) ? this.stream_id.getValue() : "";
     * 
     * PrecisionUTCTime:
     *    To create a PrecisionUTCTime object, use the following code:
     * 		  long tmp_time = System.currentTimeMillis();
     * 		  double wsec = tmp_time / 1000;
     * 		  double fsec = tmp_time % 1000;
     * 		  PrecisionUTCTime tstamp = new PrecisionUTCTime(BULKIO.TCM_CPU.value, (short)1, (short)0, wsec, fsec);
     * 
     * Ports:
     * 
     *    Each port instance is accessed through members of the following form: this.port_<PORT NAME>
     * 
     *    Data is obtained in the run function through the getPacket call (BULKIO only) on a
     *    provides port member instance. The getPacket function call is non-blocking; it takes
     *    one argument which is the time to wait on new data. If you pass 0, it will return
     *    immediately if no data available (won't wait).
     *    
     *    To send data, call the appropriate function in the port directly. In the case of BULKIO,
     *    convenience functions have been added in the port classes that aid in output.
     *    
     *    Interactions with non-BULKIO ports are left up to the component developer's discretion.
     *    
     * Properties:
     * 
     *    Properties are accessed through members of the same name with helper functions. If the 
     *    property name is baudRate, then reading the value is achieved by: this.baudRate.getValue();
     *    and writing a new value is achieved by: this.baudRate.setValue(new_value);
     *    
     * Example:
     * 
     *    This example assumes that the component has two ports:
     *        - A provides (input) port of type BULKIO::dataShort called dataShort_in
     *        - A uses (output) port of type BULKIO::dataFloat called dataFloat_out
     *    The mapping between the port and the class is found the class of the same name.
     *    This example also makes use of the following Properties:
     *        - A float value called amplitude with a default value of 2.0
     *        - A boolean called increaseAmplitude with a default value of true
     *    
     *    BULKIO_dataShortInPort.Packet<short[]> data = this.port_dataShort_in.getPacket(125);
     *
     *    if (data != null) {
     *        float[] outData = new float[data.getData().length];
     *        for (int i = 0; i < data.getData().length; i++) {
     *            if (this.increaseAmplitude.getValue()) {
     *                outData[i] = (float)data.getData()[i] * this.amplitude.getValue();
     *            } else {
     *                outData[i] = (float)data.getData()[i];
     *            }
     *        }
     *
     *        // NOTE: You must make at least one valid pushSRI call
     *        if (data.sriChanged()) {
     *            this.port_dataFloat_out.pushSRI(data.getSRI());
     *        }
     *        this.port_dataFloat_out.pushPacket(outData, data.getTime(), data.getEndOfStream(), data.getStreamID());
     *    }
     *      
     * <!-- end-user-doc -->
     * 
     * @generated
     */
    public void run() 
    {
        //begin-user-code
        //end-user-code
        
        while(this.started())
        {
            //begin-user-code
            // Process data here
            try {
            	BULKIO_dataDoubleInPort.Packet<double[]> data = this.port_dataDouble_in.getPacket(-1);
               
                if (data !=null) {
                	if (data.sriChanged() || (!this.port_dataDouble_out.hasSri(data.getStreamID()))) {
                        this.port_dataDouble_out.pushSRI(data.getSRI());
                    }	
                	
                	for (int i =0; i<data.getData().length; i++) {
                		if (data.getData()[i] > this.upper_limit.getValue()) {
                			data.getData()[i] = this.upper_limit.getValue();
                		}
                		else if (data.getData()[i] < this.lower_limit.getValue()) {
                			data.getData()[i] = this.lower_limit.getValue();
                		}
                		
                	}
                	this.port_dataDouble_out.pushPacket(data.getData(), data.getTime(), data.getEndOfStream(), data.getStreamID());
                }
                else {
                	Thread.sleep(100);
                }
            	
                
            } catch (InterruptedException e) {
                break;
            }
            
            //end-user-code
        }
        
        //begin-user-code
        //end-user-code
    }

        
    /**
     * The main function of your component.  If no args are provided, then the
     * CORBA object is not bound to an SCA Domain or NamingService and can
     * be run as a standard Java application.
     * 
     * @param args
     * @generated
     */
    public static void main(String[] args) 
    {
        final Properties orbProps = new Properties();

        //begin-user-code
        // TODO You may add extra startup code here, for example:
        // orbProps.put("com.sun.CORBA.giop.ORBFragmentSize", Integer.toString(fragSize));
        //end-user-code

        try {
            Resource.start_component(HardLimit.class, args, orbProps);
        } catch (InvalidObjectReference e) {
            e.printStackTrace();
        } catch (NotFound e) {
            e.printStackTrace();
        } catch (CannotProceed e) {
            e.printStackTrace();
        } catch (InvalidName e) {
            e.printStackTrace();
        } catch (ServantNotActive e) {
            e.printStackTrace();
        } catch (WrongPolicy e) {
            e.printStackTrace();
        } catch (InstantiationException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        }

        //begin-user-code
        // TODO You may add extra shutdown code here
        //end-user-code
    }
}