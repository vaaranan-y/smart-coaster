const express = require('express');
const router = express.Router();
const { getReadings, getReadingsByTimeFrame, getWaterConsumed, getAverageWaterConsumed } = require('../controllers/readingsController');

// GET http://localhost:8080/api/readings
router.get('/', getReadings);

// GET http://localhost:8080/api/readings/date?startStamp=1731813415&endStamp=1731813567
router.get('/date', getReadingsByTimeFrame);

// GET http://localhost:8080/api/readings/day?dayStamp=2024-11-16
router.get('/day', getWaterConsumed);

// GET http://localhost:8080/api/readings/week
router.get('/week', getAverageWaterConsumed)

module.exports = router;
