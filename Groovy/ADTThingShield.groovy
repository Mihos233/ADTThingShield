metadata {
	// Automatically generated. Make future change here.
	definition (name: "ADTThingShield", namespace: "Mihos233", author: "Mark Bowling") {
   		capability "Polling"
        capability "Sensor"
        capability "Contact Sensor"
        capability "Motion Sensor"
        
        attribute "fdoor", "string"
        attribute "bdoor", "string"
        attribute "gdoor", "string"

		fingerprint profileId: "0104", deviceId: "0138", inClusters: "0000"
	}
    
	// UI tile definitions
	tiles {
		
        standardTile("motion", "device.motion", width: 1, height: 1) {
			state "active", label:'motion', icon:"st.motion.motion.active", backgroundColor:"#53a7c0"
			state "inactive", label:'no motion', icon:"st.motion.motion.inactive", backgroundColor:"#ffffff"
		}
        standardTile("fdoor", "device.fdoor", width: 1, height: 1) {
			state("openfront", label:'Front Door', icon:"st.contact.contact.open", backgroundColor:"#ffa81e")
			state("closedfront", label:'Front Door', icon:"st.contact.contact.closed", backgroundColor:"#79b821")
		}
        standardTile("bdoor", "device.bdoor", width: 1, height: 1) {
			state("openback", label:'Back Door', icon:"st.contact.contact.open", backgroundColor:"#ffa81e")
			state("closedback", label:'Back Door', icon:"st.contact.contact.closed", backgroundColor:"#79b821")
		}
    	standardTile("gdoor", "device.gdoor", width: 1, height: 1) {
			state("opengarage", label:'Garage Door', icon:"st.contact.contact.open", backgroundColor:"#ffa81e")
			state("closedgarage", label:'Garage Door', icon:"st.contact.contact.closed", backgroundColor:"#79b821")
		}

		}

		main (["motion"])
		details(["motion","fdoor","bdoor","gdoor"])
	}

Map parse(String description) {
	def value = zigbee.parse(description)?.text
	if (value == "ping" || value == " ") 
	{
		return
	}
	
	def linkText = getLinkText(device)
	def descriptionText = getDescriptionText(description, linkText, value)
	def handlerName = value
	def isStateChange = value != "ping"
	def displayed = value && isStateChange
 
	def result = [
		value: value,
		handlerName: handlerName,
		linkText: linkText,
		descriptionText: descriptionText,
		isStateChange: isStateChange,
		displayed: displayed
	]

	if (value && value == "active") {
		result.name = "motion";
		result.value = "active";
	}else if (value && value == "inactive") {
		result.name = "motion";
		result.value = "inactive";
		
    	}else if (value && value == "fdooropen") {
		result.name = "fdoor";
		result.value = "openfront";
    	}else if (value && value == "fdoorclosed") {
		result.name = "fdoor";
		result.value = "closedfront";
        
	}else if (value && value == "bdooropen") {
		result.name = "bdoor";
		result.value = "openback";
	}else if (value && value == "bdoorclosed") {
		result.name = "bdoor";
		result.value = "closedback";
        
    	}else if (value && value == "gdooropen") {
		result.name = "gdoor";
		result.value = "opengarage";
    	}else if (value && value == "gdoorclosed") {
		result.name = "gdoor";
		result.value = "closedgarage";
        
	} else {
		result.name = null; 
	}
	createEvent(name: result.name, value: result.value)
}

def poll() {
	zigbee.smartShield(text: "poll").format()
}
