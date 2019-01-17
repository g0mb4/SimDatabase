-- phpMyAdmin SQL Dump
-- version 4.8.4
-- https://www.phpmyadmin.net/
--
-- Gép: 127.0.0.1
-- Létrehozás ideje: 2019. Jan 17. 20:15
-- Kiszolgáló verziója: 10.1.37-MariaDB
-- PHP verzió: 7.3.0

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Adatbázis: `sim_test`
--

-- --------------------------------------------------------

--
-- Tábla szerkezet ehhez a táblához `data`
--

CREATE TABLE `data` (
  `id` bigint(20) NOT NULL,
  `date` date NOT NULL,
  `time` time NOT NULL,
  `sensors` text COLLATE utf8_bin NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

--
-- A tábla adatainak kiíratása `data`
--

INSERT INTO `data` (`id`, `date`, `time`, `sensors`) VALUES
(1, '2018-04-17', '06:00:00', 'I=53.33;TW=20.98'),
(2, '2018-04-17', '06:40:00', 'I=103.33;TW=21.95'),
(3, '2018-04-17', '08:20:00', 'I=266.67;TW=24.15'),
(4, '2018-04-17', '09:15:00', 'I=363.33;TW=24.88'),
(5, '2018-04-17', '10:10:00', 'I=436.67;TW=26.34'),
(6, '2018-04-17', '10:55:00', 'I=353.33;TW=27.32'),
(7, '2018-04-17', '11:50:00', 'I=436.67;TW=28.54'),
(8, '2018-04-17', '13:35:00', 'I=606.67;TW=28.54'),
(9, '2018-04-17', '14:20:00', 'I=126.67;TW=27.56'),
(10, '2018-04-17', '15:15:00', 'I=483.33;TW=28.05'),
(11, '2018-04-17', '16:00:00', 'I=373.33;TW=27.07'),
(12, '2018-04-17', '16:55:00', 'I=283.33;TW=27.07'),
(13, '2018-04-17', '17:50:00', 'I=176.67;TW=26.58'),
(14, '2018-04-17', '18:35:00', 'I=103.33;TW=26.58'),
(15, '2018-04-17', '19:30:00', 'I=33.33;TW=25.85'),
(16, '2018-04-17', '20:25:00', 'I=6.67;TW=25.12');

--
-- Indexek a kiírt táblákhoz
--

--
-- A tábla indexei `data`
--
ALTER TABLE `data`
  ADD PRIMARY KEY (`id`);

--
-- A kiírt táblák AUTO_INCREMENT értéke
--

--
-- AUTO_INCREMENT a táblához `data`
--
ALTER TABLE `data`
  MODIFY `id` bigint(20) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=17;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
