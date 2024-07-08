/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <ExportProjectCtrl.h>
#include <ScreensCtrl.h>
#include <ScreenHeaderWidget.h>
#include <ProjectUtils.h>
#include <DownloadController.h>

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QStackedWidget>
#include <QLabel>
#include <QSizePolicy>
#include <QFileInfo>

namespace O3DE::ProjectManager
{
    ExportProjectCtrl::ExportProjectCtrl([[maybe_unused]] DownloadController* downloadController, QWidget* parent)
        : ScreenWidget(parent)
    {
        QVBoxLayout* vLayout = new QVBoxLayout();
        vLayout->setContentsMargins(0,0,0,0);

        m_header = new ScreenHeader(this);
        m_header->setTitle(tr("Export Project"));
        connect(m_header->backButton(), &QPushButton::clicked, this, &ExportProjectCtrl::HandleBackButton);
        vLayout->addWidget(m_header);

        m_stack = new QStackedWidget(this);
        m_stack->setObjectName("body");
        m_stack->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding));

        vLayout->addWidget(m_stack);

        QDialogButtonBox* buttons = new QDialogButtonBox();
        buttons->setObjectName("footer");
        vLayout->addWidget(buttons);

        m_primaryButton = buttons->addButton(tr("Export Project"), QDialogButtonBox::ApplyRole);
        m_primaryButton->setProperty("primary", true);
        connect(m_primaryButton, &QPushButton::clicked, this, &ExportProjectCtrl::HandlePrimaryButton);
 
        m_secondaryButton = buttons->addButton(tr("Back"), QDialogButtonBox::RejectRole);
        m_secondaryButton->setProperty("secondary", true);
        m_secondaryButton->setVisible(false);
        connect(m_secondaryButton, &QPushButton::clicked, this, &ExportProjectCtrl::HandleSecondaryButton);

        Update();

        setLayout(vLayout);
    }

    ProjectManagerScreen ExportProjectCtrl::GetScreenEnum()
    {
        return ProjectManagerScreen::ExportProject;
    }

    // Called when pressing "Export Project"
    void ExportProjectCtrl::NotifyCurrentScreen()
    {
    }

    void ExportProjectCtrl::HandleBackButton()
    {
        if (m_stack->currentIndex() > 0)
        {
            PreviousScreen();
        }
        else
        {
            emit GoToPreviousScreenRequest();
        }
    }

    void ExportProjectCtrl::HandleSecondaryButton()
    {
        if (m_stack->currentIndex() > 0)
        {
            PreviousScreen();
        }
        else
        {
            NextScreen();
        }
    }

    void ExportProjectCtrl::Update()
    {
        m_header->setSubTitle(tr("Export Project Details"));
        m_primaryButton->setVisible(true);
    }

    void ExportProjectCtrl::OnChangeScreenRequest(ProjectManagerScreen screen)
    {
        if (screen == ProjectManagerScreen::ProjectGemCatalog)
        {
            HandleSecondaryButton();
        }
        else if (screen == ProjectManagerScreen::GemRepos)
        {
            NextScreen();
        }
        else
        {
            emit ChangeScreenRequest(screen);
        }
    }

    void ExportProjectCtrl::NextScreen()
    {
        if (m_stack->currentIndex() < m_stack->count())
        {
            if(auto outcome = CurrentScreenIsValid(); outcome.IsSuccess())
            {
                m_stack->setCurrentIndex(m_stack->currentIndex() + 1);
                ScreenWidget* currentScreen = static_cast<ScreenWidget*>(m_stack->currentWidget());
                if (currentScreen)
                {
                    currentScreen->NotifyCurrentScreen();
                }
                Update();
            }
            else if (!outcome.GetError().isEmpty())
            {
                QMessageBox::warning(this, tr("Cannot continue"), outcome.GetError());
            }
            else 
            {
                QMessageBox::warning(this, tr("Invalid project settings"), tr("Please correct the indicated project settings and try again."));
            }
        }
    }

    void ExportProjectCtrl::PreviousScreen()
    {
        // we don't require the current screen to be valid when moving back
        if (m_stack->currentIndex() > 0)
        {
            m_stack->setCurrentIndex(m_stack->currentIndex() - 1);
            ScreenWidget* currentScreen = static_cast<ScreenWidget*>(m_stack->currentWidget());
            if (currentScreen)
            {
                currentScreen->NotifyCurrentScreen();
            }
            Update();
        }
    }

    void ExportProjectCtrl::HandlePrimaryButton()
    {
        ExportProject();
    }

    AZ::Outcome<void, QString> ExportProjectCtrl::CurrentScreenIsValid()
    {
        return AZ::Success();
    }

    void ExportProjectCtrl::ExportProject()
    {
    }

    } // namespace O3DE::ProjectManager
