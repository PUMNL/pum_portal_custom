(function ($) {

  Drupal.behaviors.ppwh = {
    attach: function (context, settings) {

      $('code.help', context).each(function () {
        var p = $(this).parent().parent().find("label");
        $(this).html("<span class='helpbutton'>&nbsp</span><div>"+$(this).text()+"</div>");
        $(this).appendTo(p);
      });
     
      $('code.help span.helpbutton', context).click(function() {
          $(this).parent().find("div").toggle();  
      });

    }
  };

})(jQuery);
