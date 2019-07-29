$(document).ready(function(){
	
	//Product Finder
	$('form.php_finder_form').on('change','select',function(e){
		 var form = $(this).closest('form');
	     var isSelectCategory=e.target.name=="category_id"?1:0; 
		 var data =$(form).serializeArray();
		 data.push({name:"isSelectCategory",value:isSelectCategory}); 
		 $.get(finderElementsUrl,data,function(html) {
			 //$('.php_product_finder_elements').html(html);
			 $('.php_product_finder_elements').empty().append($(html));
		 } );
	});
	
	$('form.php_finder_form:first').each(function(){
		 var form = this;
	     var isSelectCategory= 0; 
		 var data = $(form).serializeArray();
		 data.push({name:"isSelectCategory",value:isSelectCategory});  
		 for(var k in urlParams){ 
		     data.push({name:k,value:urlParams[k]});
		 }  
		 $.get(finderElementsUrl,data,function(html){ 
			  //$('.php_product_finder_elements').html(html);
			   $('.php_product_finder_elements').empty().append($(html));
		 }) 
	}); 	
	
	/** Reset Product Finder Form  */
	$('.finder-reset').click(function(e){
		e.preventDefault();
		var cs=$('select[name=category_id]',this.form).get(0);
		cs.options[0].selected=true;
		$.get(finderElementsUrl,{isSelectCategory:1,category_id:0},function(html) {
			 $('.php_product_finder_elements').empty().append($(html));
		});
		
	});
	
});