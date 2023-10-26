const fs = global.get('fs'); // Access the fs module from the global context

function toggleFingerStatus(requestedId) {
  // Read the JSON file
  const filePath = 'fingerData.json'; // Update with your actual file path
  let fingerData = JSON.parse(fs.readFileSync(filePath, 'utf8'));

  // Check if the requested ID is within the valid range
  if (requestedId >= 1 && requestedId <= 1000) {
    const status = fingerData[requestedId] && fingerData[requestedId].status;

    if (status !== undefined) {
      // Toggle the status
      const newStatus = status === 'IN' ? 'OUT' : 'IN';

      // Update the status in the JSON data
      fingerData[requestedId].status = newStatus;

      // Write the updated JSON data back to the file
      fs.writeFileSync(filePath, JSON.stringify(fingerData, null, 2));

      return { payload: newStatus };
    } else {
      return { payload: 'ID not found' };
    }
  } else {
    return { payload: 'Invalid ID' };
  }
}

// Call the function to toggle the status for a specific ID
const requestedId = msg.payload; // Replace with the ID you want to toggle
const result = toggleFingerStatus(requestedId);
const payloadValue = result.payload; // Extract the payload property from the result object
const payloadString = payloadValue.toString(); // Convert the payload to a string

msg.url = "https://script.google.com/macros/s/AKfycbzDmdvQRfI_akyNlv3NX9cPYks83M2CD7BRXZAySvTiCKsty0U9KNCJfp_rvrWFT27eGg/exec?fingerid=" + requestedId + "&&stat=" + payloadString;
// return msg;
console.log(msg.url);
console.log(payloadString)

return msg; // Return the result to be used in your Node-RED flow
