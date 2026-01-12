// ============================================
// STEP 1: Import Required Libraries
// ============================================

// Express: A framework to create web servers easily
const express = require('express');

// MongoDB: Database driver to connect and work with MongoDB
const { MongoClient } = require('mongodb');

// ============================================
// STEP 2: Create the Express Application
// ============================================

const app = express();

// Middleware: Tell Express how to handle data
app.use(express.json());              // Allow reading JSON data from requests
app.use(express.static('public'));    // Serve HTML/CSS/JS files from 'public' folder

// ============================================
// STEP 3: Connect to MongoDB Database
// ============================================

const mongoUrl = 'mongodb://mongodb:27017';  // MongoDB address in Docker
let db;  // Variable to store database connection

// Connect to MongoDB
MongoClient.connect(mongoUrl)
  .then(client => {
    // Success! We're connected
    db = client.db('guestbook');  // Use database named 'guestbook'
    console.log('✅ Connected to MongoDB');
  })
  .catch(err => {
    // Failed to connect
    console.error('❌ MongoDB error:', err);
  });

// ============================================
// STEP 4: Create API Routes (Endpoints)
// ============================================

// ROUTE 1: GET all messages
// When browser requests: GET http://localhost:3000/api/messages
app.get('/api/messages', async (req, res) => {
  // Get all messages from 'messages' collection
  const messages = await db.collection('messages').find().toArray();
  
  // Send messages back to browser as JSON
  res.json(messages);
});

// app.get('/api/test_get_api', async (req, res) =>{

//   res.json("hehe");
// });

// ROUTE 2: POST (add) new message
// When browser sends: POST http://localhost:3000/api/messages
app.post('/api/messages', async (req, res) => {
  // Create a new message object
  const newMessage = {
    name: req.body.name,        // Get name from request
    message: req.body.message,  // Get message from request
    date: new Date()            // Add current date/time
  };
  
  // Save message to MongoDB
  await db.collection('messages').insertOne(newMessage);
  
  // Send confirmation back to browser
  res.json(newMessage);
});

// ============================================
// STEP 5: Start the Server
// ============================================

app.listen(3000, '0.0.0.0', () => {
  console.log('🚀 Server running on http://localhost:3000');
});

// ============================================
// SUMMARY OF WHAT THIS DOES:
// ============================================
// 1. Sets up a web server using Express
// 2. Connects to MongoDB database
// 3. Creates 2 API endpoints:
//    - GET  /api/messages → Returns all messages
//    - POST /api/messages → Saves a new message
// 4. Listens on port 3000 for requests