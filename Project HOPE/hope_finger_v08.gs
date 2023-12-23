function doGet(e) {
  var fingerid = e.parameter.fingerid; // Change 'cardno' to 'fingerid'
  var ss = SpreadsheetApp.openById("1COfm5XRalJf2QMkHUA1SL6G_CcMh140bP0hEOqq7uFU");
  var sheet = ss.getSheetByName("Database");
  var data = sheet.getDataRange().getValues();
  var studentName, rollNo, card;
  var stat; // Define 'stat' outside the if condition

  for (var i = 0; i < data.length; i++) {
    if (data[i][0] == fingerid) { // Change 'cardno' to 'fingerid'
      studentName = data[i][2];
      rollNo = data[i][1];
      card = data[i][0];
      stat = data[i][4]; // Get the current status from the database
      break;
    }
  }

  // Toggle the status value
  if (stat === "IN") {
    stat = "OUT"; // Change from 'IN' to 'OUT'
  } else {
    stat = "IN"; // Change from 'OUT' to 'IN'
  }

  if (studentName && rollNo) {
    var recordSheet = ss.getSheetByName("Records");
    var currentTime = new Date();
    var formattedDate = Utilities.formatDate(currentTime, "IST", "dd/MM/yyyy");
    var formattedTime = Utilities.formatDate(currentTime, "IST", "hh:mm a");

    // Retrieve the last recorded date from Properties
    var lastRecordedDate = PropertiesService.getScriptProperties().getProperty("lastRecordedDate");

    // Check if the date has changed
    if (formattedDate != lastRecordedDate) {
      // Add date row
      recordSheet.appendRow(["Date: " + formattedDate]);
      recordSheet.appendRow(["IdNo", "Name", "Status", "Time"]); // Header row

      // Update the last recorded date in Properties
      PropertiesService.getScriptProperties().setProperty("lastRecordedDate", formattedDate);
    }

    // Append the record
    recordSheet.appendRow([rollNo, studentName, stat, formattedTime]);

    // Update the status back to the database
    for (var i = 0; i < data.length; i++) {
      if (data[i][0] == fingerid) { // Change 'cardno' to 'fingerid'
        data[i][4] = stat;
        break;
      }
    }

    // Overwrite the existing data in the database sheet
    sheet.getRange(1, 1, data.length, data[0].length).setValues(data);

    // Create a plain text response
    var response = "Status: " + stat;

    // Set the content type to plain text
    var output = ContentService.createTextOutput(response);
    output.setMimeType(ContentService.MimeType.TEXT);

    return output;
  }
}
