import '@marcellejs/core/dist/marcelle.css';

import * as marcelle from '@marcellejs/core';





// -----------------------------------------------------------
// functional function
// -----------------------------------------------------------

//convert numbers to letters
// paramaters : string
// return letter
function toDic(label){

    return String.fromCharCode(parseInt(label,10)+65);
}


//Verify that the input syntax is correct
function checkSyntaxCorrectness(value) {
    if (value === ''){
        return false;
    }
    var listwords = value.split("\n");
    console.log(listwords);
    var re=/^[A-Za-z]+$/;
    for (var i = 0; i < listwords.length; i++) {
        var chars = listwords[i].split('');
        if (chars[chars.length-1] !== ',' || chars[0] === ','){
            return false;
        }
        for (var j = 0; j < chars.length; j++) {

            if (chars[j].match(re)== null && chars[j] !== ','){
                return false;
            }
            if (chars[j] === ' ') {
                return false;
            }
            if (chars[j] === ',' && j !== (chars.length - 1)) {
                return false;
            }
        }

    }
    return true;


}


// -----------------------------------------------------------
// load predictive model
// -----------------------------------------------------------
const classifier =  marcelle.tfjsModel({
    inputType: 'image',
    taskType: 'classification',
});
classifier.loadFromUrl("src/pre/model.json");
classifier.$training.subscribe(({ status }) => {
    if (status === 'loaded') {

                console.log("success: "+ classifier.model);
    }
});





// -----------------------------------------------------------
// Wizard of learning window
// -----------------------------------------------------------


function openWizard_learn() {
    const webcam_learning = marcelle.webcam();

    const wizard_learn = marcelle.wizard();

    const page_h = marcelle.text(
        'Now to learn the h in hello<br><br><br><img src="src/images/ManualAlphabet.png" style="width:300px;height:300px;">'
    );
    const page_e = marcelle.text(
        'Now to learn the e in hello<br><br><br><img src="src/images/ManualAlphabet.png" style="width:300px;height:300px;">'
    );

    const page_l = marcelle.text(
        'Now to learn the l in hello<br><br><br><img src="src/images/ManualAlphabet.png" style="width:300px;height:300px;">'
    );
    const page_o = marcelle.text(
        'Now to learn the o in hello<br><br><br><img src="src/images/ManualAlphabet.png" style="width:300px;height:300px;">'
    );




    const predictionStream = webcam_learning.$images.filter(()=>webcam_learning.$active.get()&&classifier.ready)
        .map(async (data) => classifier.predict((data)) ).awaitPromises()
        .map(({ label, confidences }) => {
            const conf = Object.entries(confidences);
            return {
                label: toDic(label),
                confidences: conf.reduce((x, y) => ({ ...x, [toDic(parseInt(y[0]))]: y[1] }), {}),
            };
        });


    const plotResults = marcelle.confidencePlot(predictionStream);

    wizard_learn
       .page().title("Learning").description("Learn the word 'hello'").use([page_h, webcam_learning], plotResults)
        .page().title("Learning").description("Learn the word 'hello'").use([page_e, webcam_learning], plotResults)
        .page().title("Learning").description("Learn the word 'hello'").use([page_l, webcam_learning], plotResults)
        .page().title("Learning").description("Learn the word 'hello'").use([page_o, webcam_learning], plotResults)
    ;
    wizard_learn.show();
}




// -----------------------------------------------------------
// Wizard of challenge window
// -----------------------------------------------------------

let listWord = ['HELLO', "HI"];
function openWizard_challenge() {
    const webcam_challenge = marcelle.webcam(28,28);
    let word = listWord[Math.floor(Math.random() * listWord.length)];
    const wizard_challenge = marcelle.wizard();
    const predictionStream = webcam_challenge.$images.filter(()=>webcam_challenge.$active.get()&&classifier.ready)
        .map(async (data) => classifier.predict((data)) ).awaitPromises()
        .map(({ label, confidences }) => {
            const conf = Object.entries(confidences);
            return {
                label: toDic(label),
                confidences: conf.reduce((x, y) => ({ ...x, [toDic(parseInt(y[0]))]: y[1] }), {}),
            };
        });

    const plotResults = marcelle.confidencePlot(predictionStream);
    wizard_challenge.page().title("Challenge").description("challenge the word '" + word + "'").use( webcam_challenge,plotResults);
    wizard_challenge.show();
}






    document.getElementById("image_Learn").addEventListener('click', () => {
    document.getElementById("content_learn").style.display = "flex";
    document.getElementById("content_challenge").style.display = "none";
    document.getElementById("content_option").style.display = "none";

    openWizard_learn();

});

document.getElementById("image_Challenge").addEventListener('click', () => {
    document.getElementById("content_learn").style.display = "none";
    document.getElementById("content_challenge").style.display = "block";
    document.getElementById("content_option").style.display = "none";
    openWizard_challenge();
});

document.getElementById("image_Options").addEventListener('click', () => {
    document.getElementById("content_learn").style.display = "none";
    document.getElementById("content_challenge").style.display = "none";
    document.getElementById("content_option").style.display = "flex";
});

document.getElementById("textarea_words").placeholder = "Bowl,\nHello,\n...\n\n\n\n\n\n\n\n\n\n\n\n\nFinal,";

document.getElementById("btnValidate").addEventListener('click', () => {
    let words = document.getElementById("textarea_words").value;
    if (checkSyntaxCorrectness(words)){
        var tmp  = words.split(",\n");
        tmp[tmp.length-1] = tmp[tmp.length-1].split(",")[0];
         listWord.push.apply(listWord,tmp);
         var uniqueList = [];
        for (var i = 0; i < listWord.length; i++) {
            if (uniqueList.indexOf(listWord[i]) === -1) {
                uniqueList.push(listWord[i]);
            }
        }
         listWord = uniqueList;
        marcelle.notification({
            title: 'Success',
            message: "Word added successfully",
            duration: 5000,
        });
    }else{
        marcelle.notification({
            title: 'Error',
            message: "Syntax error",
            duration: 5000,
        });

    }

});

document.getElementById("btn_result_challenge").addEventListener('click',()=>{
    openWizard_challenge();
});

