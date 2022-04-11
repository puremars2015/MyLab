

function GetPageRowHTML(n) {
    let templete = `<div id="row${n}" class="row">
                        <div class="left-panel">
                            <label>第${n}視窗</label>
                            <div class="chart-display">
                                <canvas id="chart${n}" height="400px" width="350px"></canvas>
                            </div>
                            <div class="control-panel">
                                <div>
                                    <button id="qbtn${n}" class="finger" onclick="Query(this)" value="${n}">查詢個股資料</button>
                                    <!-- input id="no${n}" type="text" value="${n}"  hidden -->
                                </div>
                                <div>
                                    <label class="lb">個股代號(例如:台積電2330):</label>
                                    <input id="qsc${n}" class="condition" type="text" placeholder="ex:2330">
                                </div>
                                <div>
                                    <label class="lb">查詢區間(例如:由今天開始向前一個月,輸入1m):</label>
                                    <input type="text "id="qd${n}" class="condition" type="text" value="1m" hidden>
                                    <input type="radio" id="1d" name="period" value="1d" checked>
                                    <label for="1d">1d</label>
                                    <input type="radio" id="5d" name="period" value="5d">
                                    <label for="5d">5d</label>
                                    <input type="radio" id="1mo" name="period" value="1mo">
                                    <label for="1mo">1mo</label>
                                </div>          
                                <div>
                                    <label class="lb">模糊化係數(例如:30,表示以30為一格,減少小波動):</label>
                                    <input id="mp${n}" class="condition" type="text" placeholder="ex:30">
                                </div>
                            </div>
                        </div>
                        <div id="rpanel${n}" class="right-panel">

                        </div>
                    </div>`;

    return templete;
}


function GetPageHTML(no) {
    let html = '';
    
    for (let i = 0; i < no; i++) {
        html += GetPageRowHTML(i);
    }

    return html;
}

function GetRightPanelHTML(n) {
    let templete = `<div>
                        <canvas id="sub-chart${n}" height="400px" width="350px"></canvas>
                    </div>`;

    return templete;
}

