// sortWorker.js
function parseDateFast(dateStr) {
    return new Date(
        dateStr.slice(6,10),
        dateStr.slice(3,5) - 1,
        dateStr.slice(0,2),
        dateStr.slice(11,13),
        dateStr.slice(14,16),
        dateStr.slice(17,19)
    ).getTime();
}

self.onmessage = function (e) {
    const data = e.data;

    // Pre-parse timestamps
    data.forEach(item => {
        item.ts = parseDateFast(item.time);
    });

    // Sort by timestamp
    data.sort((a, b) => a.ts - b.ts);

    // Send back only original row order
    const sortedIndexes = data.map(item => item.index);
    self.postMessage(sortedIndexes);
};