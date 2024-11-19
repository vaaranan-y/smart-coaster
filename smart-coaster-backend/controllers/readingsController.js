const db = require('../config/firebase');

// Fetch all readings
const getReadings = async (req, res) => {
    try {
        const snapshot = await db.ref('userData/0').once('value');
        const readings = snapshot.val();
        console.log(readings);
        res.status(201).json(readings);
    } catch (error) {
        res.status(500).json({ message: error.message });
    }
};

// Fetch readings by TimeFrame
const getReadingsByTimeFrame = async (req, res) => {
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

// Fetch water consumed on given day in ISO-8601 format
const getWaterConsumed = async (req, res) => {
    const { dayStamp } = req.query;

    const startOfDay = new Date(`${dayStamp}T00:00:00`);
    const endOfDay = new Date(`${dayStamp}T23:59:59`)

    const startStamp = Math.floor(startOfDay.getTime() / 1000).toString();
    const endStamp = Math.floor(endOfDay.getTime() / 1000).toString();

    try {
        const snapshot = await db.ref('userData/0')
                                .orderByChild('timeStamp')
                                .startAt(startStamp)
                                .endAt(endStamp)
                                .once('value');
        const readings = snapshot.val();

        masses = [];
        for(key in readings) {
            masses.push(parseFloat(readings[key].currentMass));
        }
 
        prevMass = masses[0];
        numberOfCupsDrank = 0;
        currFilledCupMass = masses[0];
        console.log(masses)

        for(let i = 1; i < masses.length; i += 1) {
            currMass = masses[i];
            
            // 5 is like a +/- threshold
            if(currMass <= 5) {
                // The glass has been picked up
                continue;
            } else {
                // The glass is on the coaster
                if(currMass > prevMass + 5) {
                    // The glass on the coaster is heavier than before, 
                    // which means it has been filled, meaning the user 
                    // finished a cup of water
                    numberOfCupsDrank += 1;
                }
                prevMass = currMass;
            }
        }

        res.json({waterConsumedDay: numberOfCupsDrank});
    } catch (error) {
        res.status(500).json({ message: error.message });
    }


};

// Fetch average water consumed in week
const getAverageWaterConsumed = async (req, res) => {
    try {
        res.status(201).json({waterConsumedWeek: 0});
    } catch (error) {
        res.status(500).json({ message: error.message });
    }
}





module.exports = {getReadings, getReadingsByTimeFrame, getWaterConsumed, getAverageWaterConsumed}
