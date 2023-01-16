function doGet(e) {
  var cardno = e.parameter.cardno;
  var ss = SpreadsheetApp.openById("1COfm5XRalJf2QMkHUA1SL6G_CcMh140bP0hEOqq7uFU");
  var sheet = ss.getSheetByName("Database");
  var data = sheet.getDataRange().getValues();
  var studentName, rollNo;
  
  for (var i = 0; i < data.length; i++) {
    if (data[i][0] == cardno) {
      studentName = data[i][1];
      rollNo = data[i][2];
      break;
    }
  }
  
  if (studentName && rollNo) {
    var recordSheet = ss.getSheetByName("Records");
    var currentTime = new Date();
    recordSheet.appendRow([studentName, rollNo, currentTime]);
  }
}

function doPost(e) {
  doGet(e);
}
