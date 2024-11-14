const express = require('express');
const router = express.Router();
const { getReadings, getReadingsByDate } = require('../controllers/readingsController');

router.get('/', getReadings);

// GET http://localhost:8080/api/readings/date?startStamp=1731552835&endStamp=1731552869
router.get('/date', getReadingsByDate);

router.get('/average', (req, res) => {
    res.send('Return average reading here');
})

module.exports = router;