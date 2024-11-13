const admin = require('firebase-admin');
const serviceAccount = require('../smart-coaster-fd238-firebase-adminsdk-1754f-334bce07db.json')

admin.initializeApp({
    credential: admin.credential.cert(serviceAccount),
    databaseURL: "https://smart-coaster-fd238-default-rtdb.firebaseio.com"
});

var db = admin.database();
module.exports = db;