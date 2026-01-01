document.addEventListener('DOMContentLoaded', function () {
    const total_devices = 30; // Your variable
    const table = document.querySelector('.devices-container');
    const buttonsPerRow = 3;
    
    for (let i = 0; i < total_devices; i++) {
      // Create a new row every 3 buttons
      if (i % buttonsPerRow === 0) {
        var row = table.insertRow();
      }
      
      // Create cell and button
      const cell = row.insertCell();
      const button = document.createElement('button');
      button.textContent = `Device ${i + 1}`;
      button.className = 'device-btn';
      
      // Optional: add click event
      button.addEventListener('click', function() {
        console.log(`Device ${i + 1} clicked`);
      });
      
      cell.appendChild(button);
    }

});