$(document).ready(function(){

    $('#CreateAct').click(function()
    {
            var username=$("#usrInput").val();
            var password=$("#pwdInput").val();
            var markers = {'User':[{'@name':username,'@password': password}]};
            var serverIp = document.domain;
            $.ajax({
                    url: serverIp + "/controller/RegisterUser",
                    type: 'POST',
                    async: false,
                    
                    data: JSON.stringify(markers),
                    contentType: 'application/json',
                    
                    success: function (){
                            window.location="download.html";
                            console.log("Register User call successful");
                    },
                    error: function (e){
                            alert("Register User Call Failed");
                            console.log("Register User call failed" + e);
                    },
            });
    });
});