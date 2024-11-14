const db = require('../config/firebase');

// Fetch all readings
const getReadings = async (req, res) => {
    try {
        const snapshot = await db.ref('userData/0').once('value');
        const readings = snapshot.val();
        console.log(readings);
        res.json(readings);
    } catch (error) {
        res.status(500).json({ message: error.message });
    }
};

// Fetch readings by TimeFrame
const getReadingsByDate = async (req, res) => {
    const { startStamp, endStamp } = req.query; // Timestamps must be in epoch time
    try {
        const snapshot = await db.ref('userData/0')
                                .orderByChild('timeStamp')
                                .startAt(startStamp)
                                .endAt(endStamp)
                                .once('value');
        const readings = snapshot.val();
        console.log(readings);
        res.json(readings);
    } catch (error) {
        res.status(500).json({ message: error.message });
    }
};

module.exports = {getReadings, getReadingsByDate}