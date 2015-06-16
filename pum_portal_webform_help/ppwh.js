(function ($) {

  Drupal.behaviors.ppwh = {
    attach: function (context, settings) {

      $('code.help', context).each(function () {
        var p = $(this).parent().parent().children("label");
        $(this).html("<span class='helpbutton'>&nbsp</span><div>"+$(this).text()+"</div>");
        $(this).appendTo(p);
        $(this).find("div").append("<span class='close'>Close</span>");
      });
     
      $('code.help span.helpbutton', context).click(function() {
          $(this).parent().find("div").toggle();  
      });
      
      $('code.help span.close').click(function(){
          $(this).parent().hide();
      });

    }
  };

})(jQuery);
