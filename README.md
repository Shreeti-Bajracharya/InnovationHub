# Innovation Hub – Hackathon Collaboration Platform

## Project Overview

Innovation Hub is a collaborative web application developed using **Drupal 7.95**. The platform allows users to explore hackathon events, submit innovative project ideas, vote for ideas, and collaborate with other participants.

The project demonstrates the use of **Drupal CMS architecture**, including content types, taxonomy classification, views, custom module development, and entity relationships.

The system is designed to simulate a real-world platform where innovators can share ideas and participate in hackathon events.

---

## Key Features

* Hackathon event listing and management
* Project idea submission system
* Idea voting functionality
* Idea moderation and approval workflow
* Analytics dashboard for idea statistics
* Trending ideas block
* Entity reference relationships between events and ideas
* Taxonomy-based classification (Technology, Category, Difficulty)
* Views for dynamic content display
* Webform for feedback and submissions

---

## Technologies Used

* **Drupal 7.95**
* PHP
* MySQL
* Apache (XAMPP)
* Drupal Modules (Views, Entity Reference, Webform)

---

## Project Architecture

### Content Types

The platform uses Drupal content types to structure the data.

**1. Hackathon Event**
Fields:

* Event Title
* Event Description
* Event Date
* Organizer
* Location
* Banner Image

**2. Project Idea**
Fields:

* Idea Title
* Description
* Technology
* Category
* Difficulty Level
* Status
* Vote Count
* Event Reference

---

## Taxonomy Structure

Taxonomy vocabularies are used to categorize ideas and events.

**Technology**

* AI
* Web Development
* Blockchain
* IoT

**Category**

* Healthcare
* Education
* Environment
* Fintech

**Difficulty**

* Beginner
* Intermediate
* Advanced

These vocabularies allow filtering and classification of ideas.

---

## Entity Relationship

The system uses **Entity Reference** to connect project ideas with hackathon events.

Relationship Structure:

Hackathon Event → Project Idea

One hackathon event can contain **multiple project ideas**, while each idea belongs to a specific event.

Example:

Event: AI Innovation Hackathon
Ideas:

* AI Resume Analyzer
* Mental Health Chatbot
* Smart Agriculture System

This relationship allows structured collaboration between events and submitted ideas.

---

## Custom Module

### Module Name

innovation_hub

A custom Drupal module was developed to extend the functionality of the platform.

### Features of the Custom Module

**1. Idea Voting System**
Users can vote for project ideas. The vote count is stored in the database and displayed on the idea page.

**2. Trending Ideas Block**
Displays the top ideas based on vote count. This block is shown on the homepage.

**3. Analytics Dashboard**
Provides statistics about the system, including:

* Ideas by technology
* Ideas submitted per event
* Monthly idea submissions
* Top contributors

**4. Dynamic Homepage Content**
Loads trending ideas and upcoming events dynamically from the database.

**5. Activity Logging**
Idea submissions and voting actions are logged using Drupal’s watchdog system.

---

## Views Implementation

Drupal Views module is used to display structured data.

Main views created:

**Events Page**
URL: `/events`
Displays all hackathon events with sorting and pagination.

**Ideas Page**
URL: `/ideas`
Displays all project ideas with filtering options.

Views help present dynamic content without writing complex queries.

---

## Installation Guide

### 1. Install XAMPP

Download and install XAMPP and start:

* Apache
* MySQL

### 2. Download Drupal

Download **Drupal 7.95** and place it inside the XAMPP `htdocs` folder.

Example path:
C:\xampp\htdocs\innovationhub

### 3. Create Database

Open **phpMyAdmin** and create a database:

innovation_hub

### 4. Run Installation

Open the browser:

http://localhost/innovationhub

Follow the Drupal installation steps and connect the database.

### 5. Enable Required Modules

Enable the following modules:

* Views
* Entity Reference
* Webform
* Custom Module (innovation_hub)

---

## Database File

The repository includes the exported database file:

innovation_hub.sql

Import this file using **phpMyAdmin** to run the project with existing data.

---

## Administrator Credentials

Administrator credentials are provided in the file:

credentials.txt



---

## Future Improvements

Possible improvements for the system include:

* Real-time collaboration between users
* Comment system for ideas
* Notification system
* Advanced analytics dashboard
* API integration for external tools

---

## Author

Shreeti Bajracharya

Virtual Training Session for Training to Hire Program
Associate Software Engineer (PHP/Drupal)
