window.onload = function() {
	var images = [
		{main: 'cam1', modal: 'cam1_modal'},
		{main: 'cam2', modal: 'cam2_modal'},
		{main: 'cloud', modal: 'cloud_modal'},
		{main: 'ambient', modal: 'ambient_modal'}
	];

	var initImages = function () {
		images.forEach(function (image) {
			image.mainElement = document.getElementById(image.main);
			image.modalElement = document.getElementById(image.modal);
		});
	};

	var updateImages = function () {
		images.forEach(function (image) {
			image.mainElement.src =
				image.mainElement.src.split("?")[0] + "?" + new Date().getTime();
			image.modalElement.src =
				image.modalElement.src.split("?")[0] + "?" + new Date().getTime();
		});
	};

	function updateTextData() {
		$('#twilight_info').load('/twilight_info.txt?' + new Date().getTime());
		$('#sky_temp').load('/sky_temp.txt?' + new Date().getTime());
		$('#current_cond').load('/current_cond.txt?' + new Date().getTime());
		$('#air_temp').load('/air_temp.txt?' + new Date().getTime());
		$('#air_humid').load('/air_humid.txt?' + new Date().getTime());
	}

	initImages();
	updateTextData();

	setInterval(updateTextData, 40000);
	setInterval(updateImages, 40000);
}

