<!DOCTYPE html>
<html>
<head>

	<meta charset="utf-8">

	<title>AllSky camera web interface - System info</title>

	<!-- Bootstrap core CSS -->
	<link href="css/bootstrap.min.css" rel="stylesheet">

	<!-- Custom styles -->
	<link href="css/sticky-footer.css" rel="stylesheet">
	<link href="css/gallery.css" rel="stylesheet">
	<link href="css/main.css" rel="stylesheet">

	<!-- JS stuffs -->
	<script src="js/jquery-3.2.1.min.js"></script>
	<script src="js/bootstrap.min.js"></script>

	<!-- Meta -->
	<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
	<meta name="description" content="Allsky camera web interface. Crimean astrophysical observatory">
	<meta name="author" content="Oleg Kutkov">

	<link rel="apple-touch-icon" sizes="180x180" href="apple-touch-icon.png">
	<link rel="icon" type="image/png" sizes="32x32" href="favicon-32x32.png">
	<link rel="icon" type="image/png" sizes="16x16" href="favicon-16x16.png">
	<link rel="manifest" href="manifest.json">
	<link rel="mask-icon" href="safari-pinned-tab.svg" color="#5bbad5">
	<meta name="theme-color" content="#ffffff">

</head>

<body>
<nav class="navbar navbar-default">
	<div class="container-fluid">
		<div class="navbar-header">
			<a class="navbar-brand" href="index.html">AllSky 2.0</a>
		</div>
		<ul class="nav navbar-nav">
			<li><a href="index.html">Main</a></li>
			<li><a href="sensors.html">Sensors</a></li>
			<li class="active"><a href="#">System</a></li>
			<li><a href="about.html">About</a></li>
		</ul>
		<ul class="nav navbar-nav navbar-right">
			<li>
				<a href="https://www.youtube.com/channel/UC9mot5kKa6-PNMAXGx5_Sig" target="_blank">
					<img src="yt_logo_mono_light.png" class="img-responsive" style="padding-right: 10px">
				</a>
			</li>
		</ul>
	</div>
</nav>

<div class="container container-footer-offset">
	<div class="row">
		<div class="col-md-6">
			<h4>System information & status</h4>
			<div class="table-responsive">
				<table class="table table-striped">
					<tbody>
						<tr>
							<td><strong>Board name:</strong></td>
							<td>{board_name}</td>
						</tr>
						<tr>
							<td><strong>Operating system:</strong></td>
							<td>{os_release}</td>
						</tr>
						<tr>
							<td><strong>Linux kernel version:</strong></td>
							<td>{kernel_version}</td>
						</tr>
						<tr>
							<td><strong>Uptime:</strong></td>
							<td>{system_uptime}</td>
						</tr>
					</tbody>
				</table>
				<table class="table table-striped">
					<tbody>
						<tr>
							<td><strong>CPU freq:</strong></td>
							<td>{cpu_freq}</td>
						</tr>
						<tr>
							<td><strong>System memory:</strong></td>
							<td>{sys_mem}</td>
						</tr>
					</tbody>
				</table>
				<table class="table table-striped">
					<tbody>
						<tr>
							<td><strong>Core voltage:</strong></td>
							<td>{core_volt}</td>
						</tr>
						<tr>
							<td><strong>SDRAM voltage:</strong></td>
							<td>{sdram_volt}</td>
						</tr>
					</tbody>
				</table>
				<h4>Available cameras</h4>
				<table class="table table-striped">
					<tbody>
						{camera_list}
					</tbody>
				</table>
				<h4>Available sensors</h4>
				<table class="table table-striped">
					<tbody>
						<tr>
							<td><strong>AM2302 outdoor</strong></td>
							<td>Humidity and temperature sensor</td>
						</tr>
						<tr>
							<td><strong>AM2302 in-box:</strong></td>
							<td>Humidity and temperature sensor</td>
						</tr>
						<tr>
							<td><strong>TSL2561</strong></td>
							<td>Visible and infrared light sensor</td>
						</tr>
						<tr>
							<td><strong>MLX90614</strong></td>
							<td>Infrared thermometer</td>
						</tr>
					</tbody>
				</table>
			</div>
			<h4>Disks usage</h4>
			<img id="disks" class="img-responsive layered-image"
				src="disks_usage.png">
		</div>
		<div class="col-md-6">
			<h4>Internal sensors</h4>
			<img id="cput" class="img-responsive layered-image"
				src="cpu_temperature_day.png">
			<br>
			<img id="airth" class="img-responsive layered-image"
				src="internal_dh22_day.png">
		</div>
	</div>
</div>

<footer class="footer">
	<div class="container">
		<p class="text-muted-footer text-center">
			<br><br>Camera development: <a href="mailto:kutkov.o@yandex.ru?Subject=AllSky">Oleg Kutkov</a>
			<br>Crimean astrophysical observatory, 2017
		</p>
	</div>
</footer>

</body>
</html>

