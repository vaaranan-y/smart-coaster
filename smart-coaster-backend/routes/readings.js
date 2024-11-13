const express = require('express');
const router = express.Router();
const { getReadings } = require('../controllers/readingsController');

router.get('/', getReadings)

router.get('/average', (req, res) => {
    res.send('Return average reading here');
})

module.exports = router;