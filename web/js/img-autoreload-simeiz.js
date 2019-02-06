$(document).ready(function() {
    var imageList = [
        {main: 'cloud_sensor', modal: 'cloud_sensor_modal'},
        {main: 'ambient_sensor', modal: 'ambient_sensor_modal'}
    ];
    var textList = ['sky_temp_simeiz', 'current_cond_simeiz', 'air_temp_simeiz', 'air_humid_simeiz'];
    var REFRESH_INTERVAL = 40000;

    var images = initImages(imageList);

    updateTextData();

    setInterval(updateTextData, REFRESH_INTERVAL);
    setInterval(updateImages, REFRESH_INTERVAL);

    function initImages(img) {
        return img.map(function (image) {
            return {
                mainElement: $('#' + image.main),
                modalElement: $('#' + image.modal)
            }
        });
    }

    function updateImages() {
        images.forEach(function (image) {
            updateSrc(image.mainElement);
            updateSrc(image.modalElement);
        });
    }

    function updateSrc(elem) {
        var newSrc = getSrcWithTimestamp(elem.attr('src'));
        elem.attr('src', newSrc);
    }

    function getSrcWithTimestamp(src) {
        return src.split('?')[0] + '?' + new Date().getTime()
    }

    function updateTextData() {
        var timestemp = new Date().getTime();

        textList.forEach(function (name) {
            $('#' + name).load('/' + name + '.txt?' + timestemp);
        });
    }
    }
);
