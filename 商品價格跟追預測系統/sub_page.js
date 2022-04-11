class SubPage {
    constructor(no) {
        this.no = no;
        this.BASE_API_URL = "http://220.133.234.2/";
        this.CHART_COLORS = {
            red: 'rgb(255, 99, 132)',
            orange: 'rgb(255, 159, 64)',
            yellow: 'rgb(255, 205, 86)',
            green: 'rgb(75, 192, 192)',
            blue: 'rgb(54, 162, 235)',
            purple: 'rgb(153, 102, 255)',
            grey: 'rgb(201, 203, 207)'
        };
        this.ctx = document.getElementById('sub-chart' + no);
        this.labels = [];
        this.datapoints = [];
        this.data = {
            labels: this.labels,
            datasets: [
                {
                    label: '股價數據',
                    data: this.datapoints,
                    borderColor: this.CHART_COLORS.green,
                    fill: false,
                    tension: 0
                }
            ],
            stockCode: null,
            pointTime: null
        };
        this.config = {
            type: 'line',
            data: this.data,
            options: {
                responsive: false,
                maintainAspectRatio: false,
                plugins: {
                    title: {
                        display: true,
                        text: '------------Chart Title------------'
                    }

                },
                interaction: {
                    intersect: false,
                },
                scales: {
                    x: {
                        display: true,
                        title: {
                            display: true,
                            text: 'Date'
                        }
                    },
                    y: {
                        display: true,
                        title: {
                            display: true,
                            text: 'Price'
                        }
                    }
                }
            },
        };

        this.myChart = new Chart(this.ctx, this.config);
    }

    Query(startTime,endTime) {

        let self = this;

        let scode = document.getElementById('qsc' + self.no).value;
        let qdate = document.getElementById('qd' + self.no).value;
        let mosiac = 1;


        if (!scode || !qdate) {
            alert("請輸入完整查詢資訊!");
            return;
        }

        let stockData = JSON.parse(localStorage.getItem(scode));

        let isExist = false;

        if (stockData) {
            let data = stockData;
            for (let i of data) {
                let daystr = i.Date;
                if (qdate == daystr) {
                    isExist = true;
                    break;
                }
            }
        }

        if (!isExist) {
            $.ajax({
                url: self.BASE_API_URL + "stockInfoV2",
                data: JSON.stringify({
                    time: qdate,
                    stockCode: scode
                }),
                type: "POST",
                dataType: "json",
                contentType: "application/json;charset=utf-8",
                success: function (returnData) {

                    localStorage.setItem(scode, JSON.stringify(returnData));
                    self.myChart.data.stockCode = scode;

                    self.Draw(returnData, mosiac, startTime, endTime);
                },
                error: function (xhr, ajaxOptions, thrownError) {
                    console.log(xhr.status);
                    console.log(thrownError);
                }
            });
        }
        else {
            self.myChart.data.stockCode = scode;

            self.Draw(returnData, mosiac, startTime, endTime);
        }

        // self.QueryCondition = {

        // }
    }

    Draw(stockData, mosiac, startTime, endTime) {
        let self = this;
        let dt;
        if (endTime) {
            dt = stockData.map(x => { return { time: x.Date, price: x.Close - x.Close % mosiac } }).filter(x => x.time >= startTime && x.time < endTime);
        }
        else {
            // 沒有endtime表示是最後一點
            dt = stockData.map(x => { return { time: x.Date, price: x.Close - x.Close % mosiac } }).filter(x => x.time >= startTime);
        }
        self.myChart.data.labels = dt.map(x => x.time);
        self.myChart.data.datasets[0].data = dt.map(x => x.price);
        self.myChart.config.options.plugins.title.text = stockData.title;
        self.myChart.update();
    }

    Distinct(dt) {
        let distinct = [];
        let lastone = null;

        for (let data of dt) {
            if (!lastone || lastone.price != data.price) {
                lastone = data;
                distinct.push(data);
            }
        }

        return distinct;
    }
}