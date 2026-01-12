const express = require('express');
const { MongoClient, ObjectId } = require('mongodb');

const app = express();
app.use(express.json({ limit: '100mb' }));
app.use(express.urlencoded({ limit: '100mb', extended: true }));
app.use(express.static('public'));

const mongoUrl = 'mongodb://mongodb:27017';
let db;

MongoClient.connect(mongoUrl)
  .then(client => {
    db = client.db('Indr_PT100_Logger');
    console.log('✅ Connected to MongoDB');
  })
  .catch(err => console.error('❌ MongoDB error:', err));
app.get('/Indr_PT100_Logger/timeData', async (req, res) => {
  // const start = Date.now();
  const now = new Date();
  const GMT = 7;
  const time = String(parseInt(now.getHours()) + GMT)   + ':' +
      String(now.getMinutes()).padStart(2, '0') + ':' +
      String(now.getSeconds()).padStart(2, '0');
  const date = `${String(now.getDate()).padStart(2, '0')}/${(String(now.getMonth() + 1).padStart(2, '0'))}/${now.getFullYear()}`;
      // String(now.getFullYear());
  
  const timeData ={
    "time": time,
    "date": date,
  };
  res.json(timeData);
});


// Get all devices
app.get('/Indr_PT100_Logger/devices', async (req, res) => {
  try {
    const devices = await db.collection('devices').find().sort({id: 1}).toArray();
    // const devices = await db.collection('devices').find().count();
    res.json(devices);
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

// Get device by ID
app.get('/Indr_PT100_Logger/devices/:id', async (req, res) => {
  try {

    const device = await db.collection('devices').findOne({ id: parseInt(req.params.id) });
    if (!device) {
      return res.status(404).json({ error: 'Device not found' });
    }
    res.json(device);
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

// Create new device (auto-increment ID)
app.post('/Indr_PT100_Logger/devices', async (req, res) => {
  try {
    // Get the highest ID
    const lastDevice = await db.collection('devices')
      .find()
      .sort({id: -1})
      .limit(1)
      .toArray();
    
    const nextId = lastDevice.length > 0 ? lastDevice[0].id + 1 : 1;
    
    const newDevice = {
      id: nextId,
      devId: `DEV-${String(nextId).padStart(3, '0')}`,
      temp: [],
      time: []
    };
    
    await db.collection('devices').insertOne(newDevice);
    res.json(newDevice);
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

// Create multiple devices at once
app.post('/Indr_PT100_Logger/devices/bulk', async (req, res) => {
  try {
    const count = req.body.count || 1;
    
    // Get the highest ID
    const lastDevice = await db.collection('devices')
      .find()
      .sort({id: -1})
      .limit(1)
      .toArray();
    
    const startId = lastDevice.length > 0 ? lastDevice[0].id + 1 : 1;
    
    const devices = [];
    for (let i = 0; i < count; i++) {
      const id = startId + i;
      devices.push({
        id: id,
        devId: `DEV-${String(id).padStart(3, '0')}`,
        temp: [],
        time: []
      });
    }
    
    await db.collection('devices').insertMany(devices);
    res.json({ created: count, devices });
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

// Update device data (add temp and time)
app.put('/Indr_PT100_Logger/devices/:id', async (req, res) => {
  try {
    const { temp, time } = req.body;
    
    // Build update query to append arrays
    const update = {};
    
    if (Array.isArray(temp) && temp.length > 0) {
      update.$push = { temp: { $each: temp } };
    }
    
    if (Array.isArray(time) && time.length > 0) {
      if (!update.$push) update.$push = {};
      update.$push.time = { $each: time };
    }
    
    // If nothing to update
    if (Object.keys(update).length === 0) {
      return res.status(400).json({ error: 'No valid data provided' });
    }
    
    const result = await db.collection('devices').updateOne(
      { id: parseInt(req.params.id) },
      update
    );
    
    if (result.matchedCount === 0) {
      return res.status(404).json({ error: 'Device not found' });
    }
    
    const updatedDevice = await db.collection('devices').findOne({ id: parseInt(req.params.id) });
    res.json(updatedDevice);
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

// Replace entire temp and time arrays
app.put('/Indr_PT100_Logger/devices/:id/replace', async (req, res) => {
  try {
    const { temp, time } = req.body;
    
    const result = await db.collection('devices').updateOne(
      { id: parseInt(req.params.id) },
      { $set: { temp: temp || [], time: time || [] } }
    );
    
    if (result.matchedCount === 0) {
      return res.status(404).json({ error: 'Device not found' });
    }
    
    const updatedDevice = await db.collection('devices').findOne({ id: parseInt(req.params.id) });
    res.json(updatedDevice);
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

// Delete device
app.delete('/Indr_PT100_Logger/devices/:id', async (req, res) => {
  try {
    const result = await db.collection('devices').deleteOne({ id: parseInt(req.params.id) });
    
    if (result.deletedCount === 0) {
      return res.status(404).json({ error: 'Device not found' });
    }
    
    res.json({ success: true, message: 'Device deleted' });
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

// Delete all devices
app.delete('/Indr_PT100_Logger/devices', async (req, res) => {
  try {
    const result = await db.collection('devices').deleteMany({});
    res.json({ success: true, deletedCount: result.deletedCount });
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

app.listen(3000, '0.0.0.0', () => {
  console.log('🚀 Device Manager running on http://localhost:3000');
});