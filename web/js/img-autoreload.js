window.onload = function() {
	function updateImage() {
		var cam1_image = document.getElementById('cam1');
		var cam2_image = document.getElementById('cam2');
		var cloud_plot = document.getElementById('cloud');
		var ambient_plot = document.getElementById('ambient');

		var cam1_image_modal = document.getElementById('cam1_modal');
		var cam2_image_modal = document.getElementById('cam2_modal');
		var cloud_plot_modal = document.getElementById('cloud_modal');
		var ambient_plot_modal = document.getElementById('ambient_modal');

		cam1_image.src = cam1_image.src.split("?")[0] + "?" + new Date().getTime();
		cam2_image.src = cam2_image.src.split("?")[0] + "?" + new Date().getTime();
		cloud_plot.src = cloud_plot.src.split("?")[0] + "?" + new Date().getTime();
		ambient_plot.src = ambient_plot.src.split("?")[0] + "?" + new Date().getTime();

		cam1_image_modal.src = cam1_image_modal.src.split("?")[0] + "?" + new Date().getTime();
		cam2_image_modal.src = cam2_image_modal.src.split("?")[0] + "?" + new Date().getTime();

		cloud_plot_modal.src = cloud_plot_modal.src.split("?")[0] + "?" + new Date().getTime();
		ambient_plot_modal.src = ambient_plot_modal.src.split("?")[0] + "?" + new Date().getTime();
	}

	function updateTextData() {
		$('#sky_temp').load('/sky_temp.txt?' + new Date().getTime());
		$('#current_cond').load('/current_cond.txt?' + new Date().getTime());
		$('#air_temp').load('/air_temp.txt?' + new Date().getTime());
	}

	updateTextData();

	setInterval(updateTextData, 40000);
	setInterval(updateImage, 40000);
}

