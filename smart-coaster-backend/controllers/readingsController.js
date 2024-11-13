const db = require('../config/firebase');

// Fetch all readings
const getReadings = async (req, res) => {
    try {
        const snapshot = await db.ref('test').once('value');
        const readings = snapshot.val();
        console.log(readings);
        res.json(readings);
    } catch (error) {
        res.status(500).json({ message: error.message });
    }
};

module.exports = {getReadings}