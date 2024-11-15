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
    console.log(startStamp);
    console.log(endStamp);

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

const getWaterConsumedOnDay = async (req, res) => {
    const { dayStamp } = req.query;

    const startOfDay = new Date(`${dayStamp}T00:00:00`);
    const endOfDay = new Date(`${dayStamp}T23:59:59`)

    const startStamp = Math.floor(startOfDay.getTime() / 1000).toString();
    const endStamp = Math.floor(endOfDay.getTime() / 1000).toString();

    console.log(startStamp)
    console.log(endStamp)
    // res.json({'res': 'ok'});

    try {
        const snapshot = await db.ref('userData/0')
                                .orderByChild('timeStamp')
                                .startAt(startStamp)
                                .endAt(endStamp)
                                .once('value');
        const readings = snapshot.val();
        console.log(readings);

        for (let reading in readings) {
            console.log(reading);
        }

        res.json(readings);
    } catch (error) {
        res.status(500).json({ message: error.message });
    }


};

module.exports = {getReadings, getReadingsByDate, getWaterConsumedOnDay}