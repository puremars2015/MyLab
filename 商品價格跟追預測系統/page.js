class Page {
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
        this.ctx = document.getElementById('chart' + no);
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

        this.ctx.onclick = (evt) => {
            const points = this.myChart.getElementsAtEventForMode(
                evt,
                'nearest',
                { intersect: true },
                true);
            if (points.length) {
                let scode = document.getElementById('qsc'+this.no).value;
                let mosiac = new Number(document.getElementById('mp'+this.no).value);
                let stockData = JSON.parse(localStorage.getItem(scode));
                this.OnPointClickEvent(this.no,this.myChart.data.labels[points[0].index],this.myChart.data.labels[points[0].index + 1]);
            }
        }
    }

    OnPointClickEvent;

    Query(condition) {

        let self = this;

        let scode;
        let qdate;
        let mosiac;

        if (!condition) {
            try
            {
                scode = document.getElementById('qsc' + self.no).value;
                qdate = document.getElementById('qd' + self.no).value;
                mosiac = new Number(document.getElementById('mp' + self.no).value);
            }
            catch
            {
                console.log("scode,qdate,mosiac without data");
                return;
            }
        }
        else {
            scode = condition.scode;
            qdate = condition.qdate;
            mosiac = condition.mosiac;
        }

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

                    self.Draw(returnData, mosiac);
                },
                error: function (xhr, ajaxOptions, thrownError) {
                    console.log(xhr.status);
                    console.log(thrownError);
                }
            });
        }
        else {
            self.myChart.data.stockCode = scode;
            self.Draw(stockData, mosiac);
        }

        self.QueryCondition = {
            scode: scode,
            qdate: qdate,
            mosiac: mosiac
        }
    }

    Draw(stockData, mosiac) {

        let self = this;

        let dt = [];

        for (let x of stockData) {

            let p = 0;
            if (x.Close % mosiac == 0) {
                p = x.Close;
            }
            else {
                p = x.Close - x.Close % mosiac + mosiac;
            }

            dt.push({
                time: x.Date,
                price: p,
                original: x.Close
            });
        }

        dt = self.Distinct(dt);

        self.myChart.data.labels = dt.map(x => x.time);
        self.myChart.data.datasets[0].data = dt.map(x => x.price);

        let maxp;
        let minp;

        for (let i = 0; i < self.myChart.data.datasets[0].data.length; i++) {

            let current = self.myChart.data.datasets[0].data[i];

            if (i == 0) {
                maxp = current;
                minp = current;
            }

            if (current > maxp) {
                maxp = current;
            }


            if (current < minp) {
                minp = current;
            }

        }

        // X軸刻度數
        let xp = self.myChart.data.labels.length;
        // Y軸刻度數
        let yp = ((maxp - minp) / mosiac + 1)
        // Y軸需要增加的刻度數
        let ypplus = xp - yp;

        let p = parseInt(ypplus / 2);

        if (maxp != minp) {
            if (ypplus % 2 == 0) {
                self.myChart.config.options.scales.y.max = maxp + p * mosiac;
                self.myChart.config.options.scales.y.min = minp - p * mosiac;
            }
            else {
                self.myChart.config.options.scales.y.max = maxp + p * mosiac;
                self.myChart.config.options.scales.y.min = minp - (p + 1) * mosiac;
            }
        }

        self.myChart.config.options.plugins.title.text = stockData.title;
        self.myChart.config.options.scales.y.ticks.stepSize = mosiac;
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