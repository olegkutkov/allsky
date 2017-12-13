$(document).ready(function() {
    var imageList = [
        {main: 'cam1', modal: 'cam1_modal'},
        {main: 'cam2', modal: 'cam2_modal'},
        {main: 'cloud', modal: 'cloud_modal'},
        {main: 'ambient', modal: 'ambient_modal'}
    ];
    var textList = ['twilight_info', 'sky_temp', 'current_cond', 'air_temp', 'air_humid'];
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
