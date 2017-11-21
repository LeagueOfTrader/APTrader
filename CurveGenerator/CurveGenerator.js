var fs = require('fs');
var parseIniFile = require('./APIniParser.js');

var discreteFrequence = 10;
var continuousDays = 100;
var initPrice = 3300.0;
var rangePercent = 0.07;
var precisionBits = 0;
var priceFloor = 2500.0;
var priceCeil = 4300.0;
var trendParam = 0.7;
var outputFile='./sim_prices.txt';

var prices = [];

function generateCurve(){
    prices = [];

    var lastClosePrice = initPrice;
    for(var i = 0; i < continuousDays; i++){
        var dayPrices = generateOneDayCurve(lastClosePrice);
        lastClosePrice = dayPrices[dayPrices.length - 1];
        prices = prices.concat(dayPrices);
    }

    //console.log(prices);
    fs.writeFile(outputFile, prices,function(err){
        if(err){
            console.log('Error');
        }
        else{
            console.log('Price generated');
        }

        process.exit();
    });
}

function generateOneDayCurve(lastClosePrice){
    var dayPrices = [];
    for(var i = 0; i < discreteFrequence; i++){
        var value = -1.0;
        while(!isPriceValid(value)){
            value = generateValue(lastClosePrice, rangePercent);
        }
        value = value.toFixed(precisionBits);
        dayPrices.push(value);
    }

    return dayPrices;
}

function isPriceValid(price){
    return (price <= priceCeil && price >= priceFloor);
}

function generateValue(base, range){
    var trend = Math.random() - trendParam;
    if(trend < 0.0) {
        trend = -1.0;
    }
    else{
        trend = 1.0;
    }
    var factor = 1.0 + Math.random() * rangePercent * trend;
    return factor * base;
}

var cfg = parseIniFile('curve.cfg');
discreteFrequence = parseInt(cfg.discreteFrequence);
continuousDays = parseInt(cfg.continuousDays);
initPrice = parseFloat(cfg.initPrice);
rangePercent = parseFloat(cfg.rangePercent);
precisionBits = parseInt(cfg.precisionBits);
priceFloor = parseFloat(cfg.priceFloor);
priceCeil = parseFloat(cfg.priceCeil);
trendParam = parseFloat(cfg.trendParam);
outputFile=cfg.outputFile;


generateCurve();