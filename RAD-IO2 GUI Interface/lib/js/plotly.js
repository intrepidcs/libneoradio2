let intervalID;
let PlotCount = 0;
let PlotSeries = {
    time:[],
    data:[],
    enables: []
};
let plot_bg_color = "#fff";
let plot_font_color = "#000";

let PlotHistory = {
    time:[],
    data:[],
    enables: PlotSeries.enables,
    maxD: 0
};

let PlotData = {
    time:[],
    data:[],
    enables: PlotHistory.enables,
    maxD: 0
};

let plotlyInit = true;

$(document).ready(function () {
    if(settings.get('plot') !="0" && (settings.get('plot') == 1 || $('#plot_load').val() == 1))
    {
        getAllPlots();
    }
});

function chartSeriesInit() {
    $('#ma_graph_main_his').hide();

    let DataSeries = [];
    let array = PlotSeries.data;
    array = array.slice(0, ((globalObj.device_found.maxID_Device - 1) * 8)+ 8);
    let ind = array.length;
    let indexArray = [];
    let timeArray = [];

    for(let i = 0; i < ind; i++)
    {
        indexArray.push(i);
        timeArray.push(PlotSeries.time);
    }

    let temparray = [];
    for (let j = 0; j < globalObj.device_found.maxID_Device; j++)
    {
        temparray[j] = [];
        for (let i = 1; i < 9; i++)
        {
            if(PlotSeries.enables[(j * 8) + (i - 1)] > 0)
            {
                temparray[j][i - 1] = {
                    x: timeArray[(j * 8) + (i - 1)],
                    y: array[(j * 8) + (i - 1)],
                    name: `device${j + 1}_bank${i}`,
                    mode: 'lines',
                    type: 'scattergl',
                    line: {
                        shape: 'linear'
                    }
                };
            }
        }

        DataSeries.push(...temparray[j]);
    }

    DataSeries = DataSeries.filter(x => x !== undefined);

    let layout = {
        autosize: true,
        showlegend: true,
        dragmode: 'pan',
        plot_bgcolor: plot_bg_color,
        paper_bgcolor: plot_bg_color,
        font:{
            color: plot_font_color
        },
        xaxis:{
            showgrid:false,
            rangemode:"normal"
        },
        yaxis:{
            zeroline:false,
            fixedrange: true
        },
        margin:{
            b: 120
        }
    };

    Plotly.newPlot('ma_graph_main', DataSeries, layout, {
        scrollZoom: false,
        displaylogo: false,
        displayModeBar: true,
        modeBarButtonsToRemove: ['sendDataToCloud','lasso2d','select2d','zoom2d','hoverClosestCartesian']
    });

    PlotSeries.time = [];
    for(let i = 0; i < ind; i++)
    {
        PlotSeries.data[i] = [];
    }

    getChannels();
}

function chartDataStream()
{
    let array = PlotSeries.data;
    array = array.slice(0, ((globalObj.device_found.maxID_Device - 1) * 8)+ 8);
    array = array.filter(function (value,index) {
        if(PlotSeries.enables[index] > 0)
        {
            return value
        }
    });

    let ind = array.length;
    let indexArray = [];
    let timeArray = [];

    for(let i = 0; i < ind; i++)
    {
        indexArray.push(i);
        timeArray.push(PlotSeries.time);
    }

    Plotly.extendTraces('ma_graph_main', {
        x: timeArray,
        y: array
    },indexArray, 1000);

    PlotSeries.time = [];
    for(let i = 0; i < 64; i++)
    {
        PlotSeries.data[i] = [];
    }
}

function clickStart()
{
    clickStop();
    $("#Plots option[value='c']").prop('selected',true);

    if(globalObj === undefined || globalObj.neoRADIO_status !== 2)
    {
        change_status_text("Please go online first");
        return;
    }

    let Value = $("#Plots").val();
    if(Value !== "c" && $('#ma_graph_main_his').children().length > 0)
    {
        $('#ma_graph_main').show();
        $('#ma_graph_main_his').hide();
        Plotly.purge('ma_graph_main_his');
    }

    let refresh_rate = refreshRate;
    if(refresh_rate < 100 || refresh_rate > 5000)
    {
        refresh_rate = 100;
    }

    if(plotlyInit)
    {
        chartSeriesInit();
        plotlyInit = false;
        console.log(refresh_rate);
    }

    console.log("start graph");
    $('.progress').removeClass('hidden');
    $('.ma_graph_sidebar').addClass('shorter');
    $('#ma_graph_main').addClass('shorter');

    intervalID = setInterval(()=>{
        chartDataStream();
    },refresh_rate);

    $('.cb_device_checkbox').prop('disabled', false);
}

function clickStop() {
    clearInterval(intervalID);
    $('.progress').addClass('hidden');
    $('.ma_graph_sidebar').removeClass('shorter');
    $('#ma_graph_main').removeClass('shorter');
}

function getChannels() {
    if(globalObj === undefined)
    {
        throw "neoRAD-IO2 not connected";
    }

    let deviceConnected = globalObj.device_found.maxID_Device;
    $(".ma_graph .ma_graph_sidebar .ma_graph_sidebar_inner").html("");

    for (let i = 0; i < deviceConnected; i++)
    {
        let deviceObject = {
            deviceType : globalObj.device_found['chainlist'][`device${i}`]['channel0']['deviceType'],
            serialNumber : globalObj.device_found['serialNumber'][i],
            enables : globalObj.device_found['chainlist'][`device${i}`]
        };
        let AppendTemplate = template2(deviceObject,i);
        $(".ma_graph .ma_graph_sidebar .ma_graph_sidebar_inner").append(`${AppendTemplate}`);
    }
}

$('.ma_graph').on('change','.cb_device_checkbox',function () {
    let toggleTrue = $(this).prop('checked');
    let toggleIndex= $(this).index('.cb_device_checkbox');
    Plotly.restyle('ma_graph_main','visible', toggleTrue, toggleIndex);
});

$(window).on('resize',function(){
    resizePlot();
});

function resizePlot() {
    Plotly.Plots.resize('ma_graph_main');
}

function newPlot(Plot) {
    if(typeof Plot !== "object")
    {
        throw "not object";
    }

    if(!('data' in Plot))
    {
        throw "wrong object";
    }

    let DataSeries = [];
    let array = Plot.data;
    array = array.slice(0, ((Plot.maxD - 1) * 8)+ 8);
    let temparray = [];
    for (let j = 0; j < Plot.maxD; j++)
    {
        temparray[j] = [];
        for (let i = 1; i < 9; i++)
        {
            if(Plot.enables[(j * 8) + (i - 1)] > 0)
            {
                if(array[(j * 8) + (i - 1)])
                {
                    temparray[j][i - 1] = {
                        x: Plot.time,
                        y: array[(j * 8) + (i - 1)],
                        name: `device${j + 1}_bank${i}`,
                        mode: 'lines',
                        type: 'scattergl',
                        line: {
                            shape: 'linear'
                        }
                    };
                }
            }
        }

        DataSeries.push(...temparray[j]);
    }

    DataSeries = DataSeries.filter(x => x.y != "");

    let layout = {
        autosize: true,
        showlegend: true,
        dragmode: 'pan',
        plot_bgcolor: plot_bg_color,
        paper_bgcolor: plot_bg_color,
        font:{
            color: plot_font_color
        },
        xaxis:{
            showgrid:false,
            rangemode:"normal"
        },
        yaxis:{
            zeroline:false,
            fixedrange: true
        },
        margin:{
            b: 120
        }
    };

    $('#ma_graph_main').hide();
    $('#ma_graph_main_his').show();
    Plotly.newPlot('ma_graph_main_his', DataSeries, layout, {
        scrollZoom: true,
        displaylogo: false,
        displayModeBar: true,
        modeBarButtonsToRemove: ['sendDataToCloud','lasso2d','select2d','zoom2d','hoverClosestCartesian']
    });
}

function addSelectOption(start) {
    let show = start.replace(".", ":");
    $("#Plots").append($('<option>', {
        value: start,
        text : show
    }));
}

function selectHistoryPlot() {
    clickStop();
    let Value = $("#Plots").val();
    if(Value === "c")
    {
        $('#ma_graph_main').show();
        $('#ma_graph_main_his').hide();
        Plotly.purge('ma_graph_main_his');
    }
    else
    {
        let Path = path.join(mypath, `IntrepidCS\/neoRAD-IO2\/PlotHistory\/${Value}.json`);
        fs.readFile(`${Path}`,function (error, data) {
            if (error) throw error;
            newPlot(JSON.parse(data));
        });
    }
}

function deleteHistoryPlot() {
    let Value = $("#Plots").val();
    let Path = path.join(mypath, `IntrepidCS\/neoRAD-IO2\/PlotHistory\/${Value}.json`);
    if(Value != "c")
    {
        fs.unlink(Path, err => {
            if (err) throw err;
        });
        $(`#Plots option[value='${Value}']`).remove();
    }
}

function getAllPlots() {
    let Path = path.join(mypath, `IntrepidCS\/neoRAD-IO2\/PlotHistory`);
    let fileArray = fs.readdirSync(Path);

    fileArray = fileArray.filter(value => value.slice(-4) == "json");
    fileArray = fileArray.map(function (value) {
        return value.slice(0, -5)
    });

    fileArray.forEach(function (value) {
        addSelectOption(value);
    })
}

function getMinPollRate() {
    $('.ma_device_channel_pullrate').each((index, el) => {
        if($(el).val() < "50")
        {
            console.log(index);
        }
    });
}