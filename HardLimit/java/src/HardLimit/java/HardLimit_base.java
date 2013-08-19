/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file distributed with this 
 * source distribution.
 * 
 * This file is part of REDHAWK Basic Components HardLimit.
 * 
 * REDHAWK Basic Components HardLimit is free software: you can redistribute it and/or modify it under the terms of 
 * the GNU Lesser General Public License as published by the Free Software Foundation, either 
 * version 3 of the License, or (at your option) any later version.
 * 
 * REDHAWK Basic Components HardLimit is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
 * PURPOSE.  See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with this 
 * program.  If not, see http://www.gnu.org/licenses/.
 */
package HardLimit.java;

import java.util.Properties;

import org.apache.log4j.Logger;

import org.omg.CosNaming.NamingContextPackage.CannotProceed;
import org.omg.CosNaming.NamingContextPackage.InvalidName;
import org.omg.CosNaming.NamingContextPackage.NotFound;
import org.omg.PortableServer.POAPackage.ServantNotActive;
import org.omg.PortableServer.POAPackage.WrongPolicy;

import CF.InvalidObjectReference;

import org.ossie.component.*;
import org.ossie.properties.*;

import bulkio.*;

/**
 * This is the component code. This file contains all the access points
 * you need to use to be able to access all input and output ports,
 * respond to incoming data, and perform general component housekeeping
 *
 * Source: HardLimit.spd.xml
 *
 * @generated
 */
public abstract class HardLimit_base extends Resource implements Runnable {
    /**
     * @generated
     */
    public final static Logger logger = Logger.getLogger(HardLimit_base.class.getName());

    /**
     * Return values for service function.
     */
    public final static int FINISH = -1;
    public final static int NOOP   = 0;
    public final static int NORMAL = 1;

    /**
     * The property upper_limit
     * Sets the upper limit threshold
     *
     * @generated
     */
    public final DoubleProperty upper_limit =
        new DoubleProperty(
            "upper_limit", //id
            null, //name
            1.0, //default value
            Mode.READWRITE, //mode
            Action.EXTERNAL, //action
            new Kind[] {Kind.CONFIGURE} //kind
            );

    /**
     * The property lower_limit
     * Sets the lower limit threshold
     *
     * @generated
     */
    public final DoubleProperty lower_limit =
        new DoubleProperty(
            "lower_limit", //id
            null, //name
            -1.0, //default value
            Mode.READWRITE, //mode
            Action.EXTERNAL, //action
            new Kind[] {Kind.CONFIGURE} //kind
            );

    // Provides/inputs
    /**
     * @generated
     */
    public InDoublePort port_dataDouble_in;

    // Uses/outputs
    /**
     * @generated
     */
    public OutDoublePort port_dataDouble_out;

    /**
     * @generated
     */
    public HardLimit_base()
    {
        super();
        addProperty(upper_limit);
        addProperty(lower_limit);

        // Provides/input
        this.port_dataDouble_in = new InDoublePort("dataDouble_in");
        this.addPort("dataDouble_in", this.port_dataDouble_in);

        // Uses/output
        this.port_dataDouble_out = new OutDoublePort("dataDouble_out");
        this.addPort("dataDouble_out", this.port_dataDouble_out);
    }

    public void run() 
    {
        while(this.started())
        {
            int state = this.serviceFunction();
            if (state == NOOP) {
                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    break;
                }
            } else if (state == FINISH) {
                return;
            }
        }
    }

    protected abstract int serviceFunction();

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
        HardLimit.configureOrb(orbProps);

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
    }
}
