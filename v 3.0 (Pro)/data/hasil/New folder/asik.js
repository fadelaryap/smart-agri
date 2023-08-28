function makeChart(players, currentPage = 1, itemsPerPage = 15) {
    var startIdx = (currentPage - 1) * itemsPerPage;
    var endIdx = Math.min(startIdx + itemsPerPage, players.length);
  
    var uniqueTanggal = [];
    var playerLabels = players.slice(startIdx, endIdx).map(function (d) {
        if (!uniqueTanggal.includes(d.tanggal)) {
        uniqueTanggal.push(d.tanggal);
        return d.tanggal + " " + d.jam; // Combine "jam" and "tanggal" with a space in between
        } else {
        return d.jam; // Show only "jam" for duplicate "tanggal" entries
        }
    });
  
    var weeksData = players.slice(startIdx, endIdx).map(function (d) {
      return +d.sens2;
    });
  
    
  
    var chart1 = new Chart('chart', {
        type: "line",
        options: {
          maintainAspectRatio: false,
          legend: {
            display: false,
          },
          scales: {
            xAxes: [{
              scaleLabel: {
                display: true,
                labelString: 'Jam'
              }
            }],
            yAxes: [{
              scaleLabel: {
                display: true,
                labelString: 'Suhu'
              }
            }]
          }
        },
        data: {
          labels: playerLabels,
          datasets: [
            {
              borderColor: "red",
              data: weeksData,
              fill: false
            }
          ]
        }
      });
  
    
  
    // Generate pagination buttons
  var totalPages = Math.ceil(players.length / itemsPerPage);
  var paginationElement = document.getElementById('pagination');
  paginationElement.innerHTML = '';

  // Previous Button
  var prevButton = document.createElement('button');
  prevButton.innerText = 'Previous';
  prevButton.disabled = currentPage === 1; // Disable the button if on the first page
  prevButton.addEventListener('click', function (event) {
    event.stopPropagation(); // Prevent the event from propagating to the parent container
    makeChart(players, currentPage - 1, itemsPerPage);
  });
  paginationElement.appendChild(prevButton);

  // Next Button
  var nextButton = document.createElement('button');
  nextButton.innerText = 'Next';
  nextButton.disabled = currentPage === totalPages; // Disable the button if on the last page
  nextButton.addEventListener('click', function (event) {
    event.stopPropagation(); // Prevent the event from propagating to the parent container
    makeChart(players, currentPage + 1, itemsPerPage);
  });
  paginationElement.appendChild(nextButton);
}

function makeChart2(players, currentPage = 1, itemsPerPage = 15) {
    var startIdx = (currentPage - 1) * itemsPerPage;
    var endIdx = Math.min(startIdx + itemsPerPage, players.length);
  
    var uniqueTanggal = [];
    var playerLabels = players.slice(startIdx, endIdx).map(function (d) {
        if (!uniqueTanggal.includes(d.tanggal)) {
        uniqueTanggal.push(d.tanggal);
        return d.tanggal + " " + d.jam; // Combine "jam" and "tanggal" with a space in between
        } else {
        return d.jam; // Show only "jam" for duplicate "tanggal" entries
        }
    });
  
  
    var weeksData2 = players.slice(startIdx, endIdx).map(function (d) {
      return +d.sens1;
    });
  
      
    var chart2 = new Chart('chart2', {
      type: "line",
      options: {
        maintainAspectRatio: false,
        legend: {
          display: false,
        
      },
      scales: {
        xAxes: [{
          scaleLabel: {
            display: true,
            labelString: 'Jam'
          }
        }],
        yAxes: [{
          scaleLabel: {
            display: true,
            labelString: 'Suhu'
          }
        }]
      }
    },
      data: {
        labels: playerLabels,
        datasets: [
          {
            borderColor: "yellow",
            data: weeksData2,
            fill: false
          }
        ]
      }
    });
  
    // Generate pagination buttons
  var totalPages = Math.ceil(players.length / itemsPerPage);
  var paginationElement2 = document.getElementById('pagination2');
  paginationElement2.innerHTML = '';

  // Previous Button
  var prevButton = document.createElement('button');
  prevButton.innerText = 'Previous';
  prevButton.disabled = currentPage === 1; // Disable the button if on the first page
  prevButton.addEventListener('click', function (event) {
    event.stopPropagation(); // Prevent the event from propagating to the parent container
    makeChart2(players, currentPage - 1, itemsPerPage);
  });
  paginationElement2.appendChild(prevButton);

  // Next Button
  var nextButton = document.createElement('button');
  nextButton.innerText = 'Next';
  nextButton.disabled = currentPage === totalPages; // Disable the button if on the last page
  nextButton.addEventListener('click', function (event) {
    event.stopPropagation(); // Prevent the event from propagating to the parent container
    makeChart2(players, currentPage + 1, itemsPerPage);
  });
  paginationElement2.appendChild(nextButton);
}

function makeChart3(players, currentPage = 1, itemsPerPage = 15) {
    var startIdx = (currentPage - 1) * itemsPerPage;
    var endIdx = Math.min(startIdx + itemsPerPage, players.length);
  
    var uniqueTanggal = [];
    var playerLabels = players.slice(startIdx, endIdx).map(function (d) {
        if (!uniqueTanggal.includes(d.tanggal)) {
        uniqueTanggal.push(d.tanggal);
        return d.tanggal + " " + d.jam; // Combine "jam" and "tanggal" with a space in between
        } else {
        return d.jam; // Show only "jam" for duplicate "tanggal" entries
        }
    });
  
  
    var weeksData3 = players.slice(startIdx, endIdx).map(function (d) {
      return +d.sens3;
    });
  
      
    var chart3 = new Chart('chart3', {
      type: "line",
      options: {
        maintainAspectRatio: false,
        legend: {
          display: false,
        
      },
      scales: {
        xAxes: [{
          scaleLabel: {
            display: true,
            labelString: 'Jam'
          }
        }],
        yAxes: [{
          scaleLabel: {
            display: true,
            labelString: 'Suhu'
          }
        }]
      }
    },
      data: {
        labels: playerLabels,
        datasets: [
          {
            borderColor: "blue",
            data: weeksData3,
            fill: false
          }
        ]
      }
    });
  
    // Generate pagination buttons
  var totalPages = Math.ceil(players.length / itemsPerPage);
  var paginationElement3 = document.getElementById('pagination3');
  paginationElement3.innerHTML = '';

  // Previous Button
  var prevButton = document.createElement('button');
  prevButton.innerText = 'Previous';
  prevButton.disabled = currentPage === 1; // Disable the button if on the first page
  prevButton.addEventListener('click', function (event) {
    event.stopPropagation(); // Prevent the event from propagating to the parent container
    makeChart3(players, currentPage - 1, itemsPerPage);
  });
  paginationElement3.appendChild(prevButton);

  // Next Button
  var nextButton = document.createElement('button');
  nextButton.innerText = 'Next';
  nextButton.disabled = currentPage === totalPages; // Disable the button if on the last page
  nextButton.addEventListener('click', function (event) {
    event.stopPropagation(); // Prevent the event from propagating to the parent container
    makeChart3(players, currentPage + 1, itemsPerPage);
  });
  paginationElement3.appendChild(nextButton);
}

function makeChart4(players, currentPage = 1, itemsPerPage = 15) {
    var startIdx = (currentPage - 1) * itemsPerPage;
    var endIdx = Math.min(startIdx + itemsPerPage, players.length);
  
    var uniqueTanggal = [];
    var playerLabels = players.slice(startIdx, endIdx).map(function (d) {
        if (!uniqueTanggal.includes(d.tanggal)) {
        uniqueTanggal.push(d.tanggal);
        return d.tanggal + " " + d.jam; // Combine "jam" and "tanggal" with a space in between
        } else {
        return d.jam; // Show only "jam" for duplicate "tanggal" entries
        }
    });
  
  
    var weeksData4 = players.slice(startIdx, endIdx).map(function (d) {
      return +d.sens4;
    });
  
      
    var chart4 = new Chart('chart4', {
      type: "line",
      options: {
        maintainAspectRatio: false,
        legend: {
          display: false,
        
      },
      scales: {
        xAxes: [{
          scaleLabel: {
            display: true,
            labelString: 'Jam'
          }
        }],
        yAxes: [{
          scaleLabel: {
            display: true,
            labelString: 'Suhu'
          }
        }]
      }
    },
      data: {
        labels: playerLabels,
        datasets: [
          {
            borderColor: "blue",
            data: weeksData4,
            fill: false
          }
        ]
      }
    });
  
    // Generate pagination buttons
  var totalPages = Math.ceil(players.length / itemsPerPage);
  var paginationElement4 = document.getElementById('pagination4');
  paginationElement4.innerHTML = '';

  // Previous Button
  var prevButton = document.createElement('button');
  prevButton.innerText = 'Previous';
  prevButton.disabled = currentPage === 1; // Disable the button if on the first page
  prevButton.addEventListener('click', function (event) {
    event.stopPropagation(); // Prevent the event from propagating to the parent container
    makeChart4(players, currentPage - 1, itemsPerPage);
  });
  paginationElement4.appendChild(prevButton);

  // Next Button
  var nextButton = document.createElement('button');
  nextButton.innerText = 'Next';
  nextButton.disabled = currentPage === totalPages; // Disable the button if on the last page
  nextButton.addEventListener('click', function (event) {
    event.stopPropagation(); // Prevent the event from propagating to the parent container
    makeChart4(players, currentPage + 1, itemsPerPage);
  });
  paginationElement4.appendChild(nextButton);
}

function makeChart5(players, currentPage = 1, itemsPerPage = 15) {
    var startIdx = (currentPage - 1) * itemsPerPage;
    var endIdx = Math.min(startIdx + itemsPerPage, players.length);
  
    var uniqueTanggal = [];
    var playerLabels = players.slice(startIdx, endIdx).map(function (d) {
        if (!uniqueTanggal.includes(d.tanggal)) {
        uniqueTanggal.push(d.tanggal);
        return d.tanggal + " " + d.jam; // Combine "jam" and "tanggal" with a space in between
        } else {
        return d.jam; // Show only "jam" for duplicate "tanggal" entries
        }
    });
  
  
    var weeksData5 = players.slice(startIdx, endIdx).map(function (d) {
      return +d.sens5;
    });
  
      
    var chart5 = new Chart('chart5', {
      type: "line",
      options: {
        maintainAspectRatio: false,
        legend: {
          display: false,
        
      },
      scales: {
        xAxes: [{
          scaleLabel: {
            display: true,
            labelString: 'Jam'
          }
        }],
        yAxes: [{
          scaleLabel: {
            display: true,
            labelString: 'Suhu'
          }
        }]
      }
    },
      data: {
        labels: playerLabels,
        datasets: [
          {
            borderColor: "blue",
            data: weeksData5,
            fill: false
          }
        ]
      }
    });
  
    // Generate pagination buttons
  var totalPages = Math.ceil(players.length / itemsPerPage);
  var paginationElement5 = document.getElementById('pagination5');
  paginationElement5.innerHTML = '';

  // Previous Button
  var prevButton = document.createElement('button');
  prevButton.innerText = 'Previous';
  prevButton.disabled = currentPage === 1; // Disable the button if on the first page
  prevButton.addEventListener('click', function (event) {
    event.stopPropagation(); // Prevent the event from propagating to the parent container
    makeChart5(players, currentPage - 1, itemsPerPage);
  });
  paginationElement5.appendChild(prevButton);

  // Next Button
  var nextButton = document.createElement('button');
  nextButton.innerText = 'Next';
  nextButton.disabled = currentPage === totalPages; // Disable the button if on the last page
  nextButton.addEventListener('click', function (event) {
    event.stopPropagation(); // Prevent the event from propagating to the parent container
    makeChart5(players, currentPage + 1, itemsPerPage);
  });
  paginationElement5.appendChild(nextButton);
}

function makeChart6(players, currentPage = 1, itemsPerPage = 15) {
    var startIdx = (currentPage - 1) * itemsPerPage;
    var endIdx = Math.min(startIdx + itemsPerPage, players.length);
  
    var uniqueTanggal = [];
    var playerLabels = players.slice(startIdx, endIdx).map(function (d) {
        if (!uniqueTanggal.includes(d.tanggal)) {
        uniqueTanggal.push(d.tanggal);
        return d.tanggal + " " + d.jam; // Combine "jam" and "tanggal" with a space in between
        } else {
        return d.jam; // Show only "jam" for duplicate "tanggal" entries
        }
    });
  
  
    var weeksData6 = players.slice(startIdx, endIdx).map(function (d) {
      return +d.sens6;
    });
  
      
    var chart6 = new Chart('chart6', {
      type: "line",
      options: {
        maintainAspectRatio: false,
        legend: {
          display: false,
        
      },
      scales: {
        xAxes: [{
          scaleLabel: {
            display: true,
            labelString: 'Jam'
          }
        }],
        yAxes: [{
          scaleLabel: {
            display: true,
            labelString: 'Suhu'
          }
        }]
      }
    },
      data: {
        labels: playerLabels,
        datasets: [
          {
            borderColor: "blue",
            data: weeksData6,
            fill: false
          }
        ]
      }
    });
  
    // Generate pagination buttons
  var totalPages = Math.ceil(players.length / itemsPerPage);
  var paginationElement6 = document.getElementById('pagination6');
  paginationElement6.innerHTML = '';

  // Previous Button
  var prevButton = document.createElement('button');
  prevButton.innerText = 'Previous';
  prevButton.disabled = currentPage === 1; // Disable the button if on the first page
  prevButton.addEventListener('click', function (event) {
    event.stopPropagation(); // Prevent the event from propagating to the parent container
    makeChart6(players, currentPage - 1, itemsPerPage);
  });
  paginationElement6.appendChild(prevButton);

  // Next Button
  var nextButton = document.createElement('button');
  nextButton.innerText = 'Next';
  nextButton.disabled = currentPage === totalPages; // Disable the button if on the last page
  nextButton.addEventListener('click', function (event) {
    event.stopPropagation(); // Prevent the event from propagating to the parent container
    makeChart6(players, currentPage + 1, itemsPerPage);
  });
  paginationElement6.appendChild(nextButton);
}

function makeChart7(players, currentPage = 1, itemsPerPage = 15) {
    var startIdx = (currentPage - 1) * itemsPerPage;
    var endIdx = Math.min(startIdx + itemsPerPage, players.length);
  
    var uniqueTanggal = [];
    var playerLabels = players.slice(startIdx, endIdx).map(function (d) {
        if (!uniqueTanggal.includes(d.tanggal)) {
        uniqueTanggal.push(d.tanggal);
        return d.tanggal + " " + d.jam; // Combine "jam" and "tanggal" with a space in between
        } else {
        return d.jam; // Show only "jam" for duplicate "tanggal" entries
        }
    });
  
  
    var weeksData7 = players.slice(startIdx, endIdx).map(function (d) {
      return +d.sens7;
    });
  
      
    var chart7 = new Chart('chart7', {
      type: "line",
      options: {
        maintainAspectRatio: false,
        legend: {
          display: false,
        
      },
      scales: {
        xAxes: [{
          scaleLabel: {
            display: true,
            labelString: 'Jam'
          }
        }],
        yAxes: [{
          scaleLabel: {
            display: true,
            labelString: 'Suhu'
          }
        }]
      }
    },
      data: {
        labels: playerLabels,
        datasets: [
          {
            borderColor: "blue",
            data: weeksData7,
            fill: false
          }
        ]
      }
    });
  
    // Generate pagination buttons
  var totalPages = Math.ceil(players.length / itemsPerPage);
  var paginationElement7 = document.getElementById('pagination7');
  paginationElement7.innerHTML = '';

  // Previous Button
  var prevButton = document.createElement('button');
  prevButton.innerText = 'Previous';
  prevButton.disabled = currentPage === 1; // Disable the button if on the first page
  prevButton.addEventListener('click', function (event) {
    event.stopPropagation(); // Prevent the event from propagating to the parent container
    makeChart7(players, currentPage - 1, itemsPerPage);
  });
  paginationElement7.appendChild(prevButton);

  // Next Button
  var nextButton = document.createElement('button');
  nextButton.innerText = 'Next';
  nextButton.disabled = currentPage === totalPages; // Disable the button if on the last page
  nextButton.addEventListener('click', function (event) {
    event.stopPropagation(); // Prevent the event from propagating to the parent container
    makeChart7(players, currentPage + 1, itemsPerPage);
  });
  paginationElement7.appendChild(nextButton);
}

function makeChart8(players, currentPage = 1, itemsPerPage = 15) {
    var startIdx = (currentPage - 1) * itemsPerPage;
    var endIdx = Math.min(startIdx + itemsPerPage, players.length);
  
    var uniqueTanggal = [];
    var playerLabels = players.slice(startIdx, endIdx).map(function (d) {
        if (!uniqueTanggal.includes(d.tanggal)) {
        uniqueTanggal.push(d.tanggal);
        return d.tanggal + " " + d.jam; // Combine "jam" and "tanggal" with a space in between
        } else {
        return d.jam; // Show only "jam" for duplicate "tanggal" entries
        }
    });
  
  
    var weeksData8 = players.slice(startIdx, endIdx).map(function (d) {
      return +d.sens8;
    });
  
      
    var chart8 = new Chart('chart8', {
      type: "line",
      options: {
        maintainAspectRatio: false,
        legend: {
          display: false,
        
      },
      scales: {
        xAxes: [{
          scaleLabel: {
            display: true,
            labelString: 'Jam'
          }
        }],
        yAxes: [{
          scaleLabel: {
            display: true,
            labelString: 'Suhu'
          }
        }]
      }
    },
      data: {
        labels: playerLabels,
        datasets: [
          {
            borderColor: "blue",
            data: weeksData8,
            fill: false
          }
        ]
      }
    });
  
    // Generate pagination buttons
  var totalPages = Math.ceil(players.length / itemsPerPage);
  var paginationElement8 = document.getElementById('pagination8');
  paginationElement8.innerHTML = '';

  // Previous Button
  var prevButton = document.createElement('button');
  prevButton.innerText = 'Previous';
  prevButton.disabled = currentPage === 1; // Disable the button if on the first page
  prevButton.addEventListener('click', function (event) {
    event.stopPropagation(); // Prevent the event from propagating to the parent container
    makeChart8(players, currentPage - 1, itemsPerPage);
  });
  paginationElement8.appendChild(prevButton);

  // Next Button
  var nextButton = document.createElement('button');
  nextButton.innerText = 'Next';
  nextButton.disabled = currentPage === totalPages; // Disable the button if on the last page
  nextButton.addEventListener('click', function (event) {
    event.stopPropagation(); // Prevent the event from propagating to the parent container
    makeChart8(players, currentPage + 1, itemsPerPage);
  });
  paginationElement8.appendChild(nextButton);
}

function makeChart9(players, currentPage = 1, itemsPerPage = 15) {
    var startIdx = (currentPage - 1) * itemsPerPage;
    var endIdx = Math.min(startIdx + itemsPerPage, players.length);
  
    var uniqueTanggal = [];
    var playerLabels = players.slice(startIdx, endIdx).map(function (d) {
        if (!uniqueTanggal.includes(d.tanggal)) {
        uniqueTanggal.push(d.tanggal);
        return d.tanggal + " " + d.jam; // Combine "jam" and "tanggal" with a space in between
        } else {
        return d.jam; // Show only "jam" for duplicate "tanggal" entries
        }
    });
  
  
    var weeksData9 = players.slice(startIdx, endIdx).map(function (d) {
      return +d.sens9;
    });
  
      
    var chart9 = new Chart('chart9', {
      type: "line",
      options: {
        maintainAspectRatio: false,
        legend: {
          display: false,
        
      },
      scales: {
        xAxes: [{
          scaleLabel: {
            display: true,
            labelString: 'Jam'
          }
        }],
        yAxes: [{
          scaleLabel: {
            display: true,
            labelString: 'Suhu'
          }
        }]
      }
    },
      data: {
        labels: playerLabels,
        datasets: [
          {
            borderColor: "blue",
            data: weeksData9,
            fill: false
          }
        ]
      }
    });
  
    // Generate pagination buttons
  var totalPages = Math.ceil(players.length / itemsPerPage);
  var paginationElement9 = document.getElementById('pagination9');
  paginationElement9.innerHTML = '';

  // Previous Button
  var prevButton = document.createElement('button');
  prevButton.innerText = 'Previous';
  prevButton.disabled = currentPage === 1; // Disable the button if on the first page
  prevButton.addEventListener('click', function (event) {
    event.stopPropagation(); // Prevent the event from propagating to the parent container
    makeChart9(players, currentPage - 1, itemsPerPage);
  });
  paginationElement9.appendChild(prevButton);

  // Next Button
  var nextButton = document.createElement('button');
  nextButton.innerText = 'Next';
  nextButton.disabled = currentPage === totalPages; // Disable the button if on the last page
  nextButton.addEventListener('click', function (event) {
    event.stopPropagation(); // Prevent the event from propagating to the parent container
    makeChart9(players, currentPage + 1, itemsPerPage);
  });
  paginationElement9.appendChild(nextButton);
}

// Request data using D3
d3.csv("https://fadelaryap.github.io/ujicoba.github.io/dataraw3.csv")
  .then(function (players) {
    makeChart(players); // Default to the first page with 20 items per page
  });

d3.csv("https://fadelaryap.github.io/ujicoba.github.io/dataraw3.csv")
.then(function (players) {
makeChart2(players); // Default to the first page with 20 items per page
});

d3.csv("https://fadelaryap.github.io/ujicoba.github.io/dataraw3.csv")
.then(function (players) {
makeChart3(players); // Default to the first page with 20 items per page
});

d3.csv("https://fadelaryap.github.io/ujicoba.github.io/dataraw4.csv")
.then(function (players) {
makeChart4(players); // Default to the first page with 20 items per page
});

d3.csv("https://fadelaryap.github.io/ujicoba.github.io/dataraw4.csv")
.then(function (players) {
makeChart5(players); // Default to the first page with 20 items per page
});

d3.csv("https://fadelaryap.github.io/ujicoba.github.io/dataraw4.csv")
.then(function (players) {
makeChart6(players); // Default to the first page with 20 items per page
});

d3.csv("https://fadelaryap.github.io/ujicoba.github.io/dataraw4.csv")
.then(function (players) {
makeChart7(players); // Default to the first page with 20 items per page
});

d3.csv("https://fadelaryap.github.io/ujicoba.github.io/dataraw4.csv")
.then(function (players) {
makeChart8(players); // Default to the first page with 20 items per page
});

d3.csv("https://fadelaryap.github.io/ujicoba.github.io/dataraw4.csv")
.then(function (players) {
makeChart9(players); // Default to the first page with 20 items per page
});