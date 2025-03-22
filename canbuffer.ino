
/********************************************************************************************/
 /* CANBUFFER
  *  A Simple buffer. Receives an event and sends an event with same event numer.
  *  A way of producing a 'self consumed' event for CANGATE
  *  Set EV1 to the value of 1 if using with output of CANAGTE
  *  EV2 must be set to a value of 1 to 5. This will determine the number of events produced
  *  EV3 is invert output. Set to 255 to invert. Leave at 0 for normal.
  *  Philip Silver MERG Member 4082
  */
/********************************************************************************************/




/*
Pins used for interface chip

           Nano  
INT         D2       
SCK   SCK   D13      
SI    MISO  D11      
SO    MOSI  D12      
CS          D10      
GND         0V       
VCC         5V      

*/
/********************************************************************************************/


/********************************************************************************************/
// Load CBUS Libraries
/********************************************************************************************/

#include <SPI.h> //equired by the CBUS library to communicate to MCP2515 CAN Controller
#include <MergCBUS.h> // Main CBUS Library
#include <Message.h>  // CBUS Message Libary
#include <EEPROM.h> //Required by the CBUS library to read / write Node Identifiction and Node Varaiables


/********************************************************************************************/





/********************************************************************************************/
//CBUS definitions
/********************************************************************************************/
  #define GREEN_LED 4               //MERG Green (SLIM) LED port
  #define YELLOW_LED 5              //MERG Yellow (FLIM) LED port
  #define PUSH_BUTTON 6             //std merg FLIM / SLIM push button
 //#define PUSH_BUTTON1 3          //debug push button
  #define NODE_VARS 0      //sets up number of NVs for module to store variables

  #define NODE_EVENTS 254    //Max Number of supported Events is 255
  #define EVENTS_VARS 3  //number of variables per event Maximum is 20
  #define DEVICE_NUMBERS 0  //number of devices numbers connected to Arduino such as servos, relays etc. Can be used for Short events

/********************************************************************************************/





/********************************************************************************************/
//Variables
/********************************************************************************************/





/********************************************************************************************/




/********************************************************************************************/
//Create the MERG CBUS object - cbus
/********************************************************************************************/

MergCBUS cbus=MergCBUS(NODE_VARS,NODE_EVENTS,EVENTS_VARS,DEVICE_NUMBERS);

/********************************************************************************************/






void setup () {


//pinMode(PUSH_BUTTON1,INPUT_PULLUP);//debug push button if required


/********************************************************************************************/
//Configuration CBUS data for the node
/********************************************************************************************/

  cbus.getNodeId()->setNodeName("BUFFER",6);       //node name shows in FCU when first detected set your own name for each module - max 8 characters
  cbus.getNodeId()->setModuleId(134);               //module number - set above 100 to avoid conflict with other MERG CBUS modules
  cbus.getNodeId()->setManufacturerId(0xA5);        //MERG code
  cbus.getNodeId()->setMinCodeVersion(0x62);        //Version b
  cbus.getNodeId()->setMaxCodeVersion(1);           // version 1
  cbus.getNodeId()->setProducerNode(true);          //Produce Node - Set True if Producer Module
  cbus.getNodeId()->setConsumerNode(true);          //Consumer Node = Set True if Consumer Module
  cbus.getNodeId()->setConsumeEvents(false);         // consume own event COE
  cbus.getNodeId()->setTransportType(1);            // 1 CAN  2 ETHERNET 3 MiWi
  cbus.getNodeId()->setBetaRelease(0);              // Beta Release set to zero for release version
  cbus.getNodeId()->setSuportBootLoading(false);    // Allways set false - no bootloading
  //cbus.getNodeId()->setCpuType(50);                  // Physical Chip see cbusdefs
  cbus.getNodeId()->setCpuManufacturer(2);          //1 MICROCHIP  2  ATMEL  3 ARM



  cbus.setStdNN(999);                               // Node Number in SLIM Mode. The default is 0 for Consumers or 1 - 99 for Producers.
  

  

/********************************************************************************************/
/* Certain node numbers are presently allocated to specific devices and should not be taught to any
    modules. These are:
    0xFFFF (65535) Used by all CABs
    0xFFFE (65534) - Used by command station
    0x007F (127) - Used by CAN_USB modules
    0x007E (126) - Used by CAN_RS modules
    0x0064 to 0x007D (101 to 125) -   Reserved for modules with fixed NNs.
*/
/********************************************************************************************/



/********************************************************************************************/
// Set ports and CAN Transport Layer
/********************************************************************************************/

  cbus.setLeds(GREEN_LED,YELLOW_LED);//set the led ports
  cbus.setPushButton(PUSH_BUTTON);//set the push button ports
  cbus.setUserHandlerFunction(&myUserFunc);//function that implements the node logic when recieving events
  cbus.initCanBus(10,CAN_125KBPS,MCP_8MHz,10,200);  //initiate the transport layer. pin=10, rate=125Kbps,10 tries,200 millis between each try
  
  //Note the clock speed 8Mhz. If 16Mhz crystal fitted change above to MCP_16Mhz
/********************************************************************************************/





//Serial.begin(115200);


} // End Of Set Up





/********************************************************************************************/
//  Functions
/********************************************************************************************/



void myUserFunc(Message *msg,MergCBUS *mcbus){
  
  // byte CBUSOpc = msg->getOpc(); // Get The OPCODE from Message
   //int nodeVariable = cbus.getNodeVar(1); // Get The Node Event Variable 1
   int eventNumber = msg->getEventNumber(); // Get The Event Number from Message
   int eventVariable1 = mcbus->getEventVar(msg,1); // Set to 1 Used only if using CANGATE Output
   int numberSendEvents = mcbus->getEventVar(msg,2); // Number of Sent Events
   int invert = mcbus->getEventVar(msg,3); // used to invert event
   //int bufferEventNumber = (eventNumber *nodeVariable);
   int nodeNumber = msg->getNodeNumber();
   int baseEvent1 = 10000;
   int baseEvent2 = 20000;
   int baseEvent3 = 30000;
   int baseEvent4 = 40000;
   int baseEvent5 = 50000;

   if (eventVariable1 == 1){
    eventNumber = (eventNumber / 10);
   }
       
   int tens = (nodeNumber / 10)  % 10;
   int units = nodeNumber % 10;
   int newNodeTens = (tens *1000);
   int newNodeUnits = (units*100);
   int newNode = (newNodeTens + newNodeUnits);

   
   if (mcbus->eventMatch()){  //The recived event has been taught this module
      switch(numberSendEvents) {  

               case 1: 
                    if (invert !=255) {
                     if (mcbus->isAccOn()== true){
                        cbus.sendOnEvent(true, (baseEvent1 + newNode + eventNumber));
                      }  
                    } 
                      if (invert !=255) {
                      if (mcbus->isAccOff()== true) {
                      cbus.sendOffEvent(true, (baseEvent1 + newNode + eventNumber));
                          }
                      }

// code for event invert
                   if (invert == 255){
                    if (mcbus->isAccOn()== true){
                        cbus.sendOffEvent(true, (baseEvent1 + newNode + eventNumber));
                        }
                     }
                     if (invert == 255){
                     if (mcbus->isAccOff()== true) {
                      cbus.sendOnEvent(true, (baseEvent1 + newNode + eventNumber));
                        }
                     }

              break;
           
			        case 2: 
                    if (invert !=255) {
                     if (mcbus->isAccOn()== true){
                        cbus.sendOnEvent(true, (baseEvent1 + newNode + eventNumber));
                        cbus.sendOnEvent(true, (baseEvent2 + newNode + eventNumber));
                      }  
                    } 
                      if (invert !=255) {
                      if (mcbus->isAccOff()== true) {
                      cbus.sendOffEvent(true, (baseEvent1 + newNode + eventNumber));
                      cbus.sendOffEvent(true, (baseEvent2 + newNode + eventNumber));
                          }
                      }

// code for event invert
                   if (invert == 255){
                    if (mcbus->isAccOn()== true){
                        cbus.sendOffEvent(true, (baseEvent1 + newNode + eventNumber));
                        cbus.sendOnEvent(true, (baseEvent2 + newNode + eventNumber));
                        }
                     }
                     if (invert == 255){
                     if (mcbus->isAccOff()== true) {
                      cbus.sendOnEvent(true, (baseEvent1 + newNode + eventNumber));
                      cbus.sendOffEvent(true, (baseEvent2 + newNode + eventNumber));
                        }
                     }

              break;

              case 3: 
                    if (invert !=255) {
                     if (mcbus->isAccOn()== true){
                        cbus.sendOnEvent(true, (baseEvent1 + newNode + eventNumber));
                        cbus.sendOnEvent(true, (baseEvent2 + newNode + eventNumber));
                        cbus.sendOnEvent(true, (baseEvent3 + newNode + eventNumber));
                      }  
                    } 
                      if (invert !=255) {
                      if (mcbus->isAccOff()== true) {
                      cbus.sendOffEvent(true, (baseEvent1 + newNode + eventNumber));
                      cbus.sendOffEvent(true, (baseEvent2 + newNode + eventNumber));
                      cbus.sendOffEvent(true, (baseEvent3 + newNode + eventNumber));
                          }
                      }

// code for event invert
                   if (invert == 255){
                    if (mcbus->isAccOn()== true){
                        cbus.sendOffEvent(true, (baseEvent1 + newNode + eventNumber));
                        cbus.sendOffEvent(true, (baseEvent2 + newNode + eventNumber));
                        cbus.sendOffEvent(true, (baseEvent3 + newNode + eventNumber));
                        }
                     }
                     if (invert == 255){
                     if (mcbus->isAccOff()== true) {
                      cbus.sendOnEvent(true, (baseEvent1 + newNode + eventNumber));
                      cbus.sendOnEvent(true, (baseEvent2 + newNode + eventNumber));
                      cbus.sendOnEvent(true, (baseEvent3 + newNode + eventNumber));
                        }
                     }

              break;

               case 4: 
                    if (invert !=255) {
                     if (mcbus->isAccOn()== true){
                        cbus.sendOnEvent(true, (baseEvent1 + newNode + eventNumber));
                        cbus.sendOnEvent(true, (baseEvent2 + newNode + eventNumber));
                        cbus.sendOnEvent(true, (baseEvent3 + newNode + eventNumber));
                        cbus.sendOnEvent(true, (baseEvent4 + newNode + eventNumber));
                      }  
                    } 
                      if (invert !=255) {
                      if (mcbus->isAccOff()== true) {
                      cbus.sendOffEvent(true, (baseEvent1 + newNode + eventNumber));
                      cbus.sendOffEvent(true, (baseEvent2 + newNode + eventNumber));
                      cbus.sendOffEvent(true, (baseEvent3 + newNode + eventNumber));
                      cbus.sendOffEvent(true, (baseEvent4 + newNode + eventNumber));
                          }
                      }

// code for event invert
                   if (invert == 255){
                    if (mcbus->isAccOn()== true){
                        cbus.sendOffEvent(true, (baseEvent1 + newNode + eventNumber));
                        cbus.sendOffEvent(true, (baseEvent2 + newNode + eventNumber));
                        cbus.sendOffEvent(true, (baseEvent3 + newNode + eventNumber));
                        cbus.sendOffEvent(true, (baseEvent4 + newNode + eventNumber));
                        }
                     }
                     if (invert == 255){
                     if (mcbus->isAccOff()== true) {
                      cbus.sendOnEvent(true, (baseEvent1 + newNode + eventNumber));
                      cbus.sendOnEvent(true, (baseEvent2 + newNode + eventNumber));
                      cbus.sendOnEvent(true, (baseEvent3 + newNode + eventNumber));
                      cbus.sendOnEvent(true, (baseEvent4 + newNode + eventNumber));
                        }
                     }

              break;

				      case 5: 
                    if (invert !=255) {
                     if (mcbus->isAccOn()== true){
                        cbus.sendOnEvent(true, (baseEvent1 + newNode + eventNumber));
                        cbus.sendOnEvent(true, (baseEvent2 + newNode + eventNumber));
                        cbus.sendOnEvent(true, (baseEvent3 + newNode + eventNumber));
                        cbus.sendOnEvent(true, (baseEvent4 + newNode + eventNumber));
                        cbus.sendOnEvent(true, (baseEvent5 + newNode + eventNumber));
                      }  
                    } 
                      if (invert !=255) {
                      if (mcbus->isAccOff()== true) {
                      cbus.sendOffEvent(true, (baseEvent1 + newNode + eventNumber));
                      cbus.sendOffEvent(true, (baseEvent2 + newNode + eventNumber));
                      cbus.sendOffEvent(true, (baseEvent3 + newNode + eventNumber));
                      cbus.sendOffEvent(true, (baseEvent4 + newNode + eventNumber));
                      cbus.sendOffEvent(true, (baseEvent5 + newNode + eventNumber));
                          }
                      }

// code for event invert
                   if (invert == 255){
                    if (mcbus->isAccOn()== true){
                        cbus.sendOffEvent(true, (baseEvent1 + newNode + eventNumber));
                        cbus.sendOffEvent(true, (baseEvent2 + newNode + eventNumber));
                        cbus.sendOffEvent(true, (baseEvent3 + newNode + eventNumber));
                        cbus.sendOffEvent(true, (baseEvent4 + newNode + eventNumber));
                        cbus.sendOffEvent(true, (baseEvent5 + newNode + eventNumber));
                        }
                     }
                     if (invert == 255){
                     if (mcbus->isAccOff()== true) {
                      cbus.sendOnEvent(true, (baseEvent1 + newNode + eventNumber));
                      cbus.sendOnEvent(true, (baseEvent2 + newNode + eventNumber));
                      cbus.sendOnEvent(true, (baseEvent3 + newNode + eventNumber));
                      cbus.sendOnEvent(true, (baseEvent4 + newNode + eventNumber));
                      cbus.sendOnEvent(true, (baseEvent5 + newNode + eventNumber));
                        }
                     }

              break;
      }
		
            
   } // End OF Recieved Events
          
    
           
  } // end function
                


/*FUNCTIONS FINISH ***********************************************************************************/



void loop() {

    cbus.run();// Run CBUS
    cbus.cbusRead(); // Check CBUS Buffers for any activity

}
