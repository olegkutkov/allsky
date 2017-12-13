$(document).ready(function () {
    var slideIndex = 0;
    var $modal = $('#modal');

    showSlides(slideIndex);

    $modal.modal({
        remote: url,
        refresh: true
    });

    function showSlides(n) {
        var $slides = $('.imgSlides');

        if (n > $slides.length) {slideIndex = 0}
        if (n < 1) {slideIndex = $slides.length - 1}

        $slides.css({display: 'none'});
        $slides[slideIndex].style.display = 'block';
    }

    function plusSlides(n) {
        showSlides(slideIndex += n);
    }

    function currentSlide(n) {
        showSlides(slideIndex = n);
    }

    function openModal() {
        $modal.show();
    }

    function closeModal() {
        $modal.hide();
    }

    window.plusSlides = plusSlides;
    window.currentSlide = currentSlide;
    window.openModal = openModal;
    window.closeModal = closeModal;
});
